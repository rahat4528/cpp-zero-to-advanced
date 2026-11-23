#include "TFIDFModel.hpp"
#include <cmath>
#include <algorithm>
#include <set>

namespace resume_ranker {

void TFIDFModel::addResume(const CandidateResume& resume) {
    CandidateResume r = resume;
    if (r.tokens.empty() && !r.rawContent.empty()) {
        r.tokens = TextTokenizer::tokenize(r.rawContent);
    }
    m_resumes.push_back(std::move(r));
}

void TFIDFModel::clear() {
    m_resumes.clear();
}

size_t TFIDFModel::getResumeCount() const noexcept {
    return m_resumes.size();
}

std::vector<RankedCandidate> TFIDFModel::rankResumes(const std::string& jobDescription) const {
    std::vector<RankedCandidate> rankings;
    if (m_resumes.empty()) return rankings;

    auto queryTokens = TextTokenizer::tokenize(jobDescription);
    if (queryTokens.empty()) return rankings;

    // Build vocabulary and Document Frequency (DF)
    // N is resumes + 1 query
    size_t totalDocs = m_resumes.size() + 1;
    std::map<std::string, int> docFreq;

    auto countDocTerms = [&docFreq](const std::vector<std::string>& tokens) {
        std::set<std::string> uniqueTerms(tokens.begin(), tokens.end());
        for (const auto& term : uniqueTerms) {
            docFreq[term]++;
        }
    };

    countDocTerms(queryTokens);
    for (const auto& r : m_resumes) {
        countDocTerms(r.tokens);
    }

    // Compute IDF for each term: idf = ln(1 + (N + 1) / (df + 1)) + 1
    std::map<std::string, double> idfMap;
    for (const auto& [term, df] : docFreq) {
        double idf = std::log(1.0 + (static_cast<double>(totalDocs + 1) / static_cast<double>(df + 1))) + 1.0;
        idfMap[term] = idf;
    }

    // Compute query TF-IDF vector
    std::map<std::string, double> queryVec;
    std::map<std::string, int> queryTf;
    for (const auto& t : queryTokens) queryTf[t]++;

    double queryNormSq = 0.0;
    for (const auto& [term, count] : queryTf) {
        double tf = static_cast<double>(count) / static_cast<double>(queryTokens.size());
        double weight = tf * idfMap[term];
        queryVec[term] = weight;
        queryNormSq += weight * weight;
    }
    double queryNorm = std::sqrt(queryNormSq);

    // Compute TF-IDF vector for each resume and Cosine Similarity
    for (const auto& r : m_resumes) {
        if (r.tokens.empty()) continue;

        std::map<std::string, int> resumeTf;
        for (const auto& t : r.tokens) resumeTf[t]++;

        double dotProduct = 0.0;
        double docNormSq = 0.0;
        std::vector<std::pair<double, std::string>> matchingWeights;

        for (const auto& [term, count] : resumeTf) {
            double tf = static_cast<double>(count) / static_cast<double>(r.tokens.size());
            double weight = tf * idfMap[term];
            docNormSq += weight * weight;

            if (queryVec.count(term)) {
                dotProduct += weight * queryVec.at(term);
                matchingWeights.push_back({weight, term});
            }
        }

        double docNorm = std::sqrt(docNormSq);
        double similarity = 0.0;
        if (queryNorm > 0.00001 && docNorm > 0.00001) {
            similarity = dotProduct / (queryNorm * docNorm);
        }

        // Sort top matching keywords
        std::sort(matchingWeights.rbegin(), matchingWeights.rend());
        std::vector<std::string> topWords;
        for (size_t i = 0; i < std::min<size_t>(5, matchingWeights.size()); ++i) {
            topWords.push_back(matchingWeights[i].second);
        }

        RankedCandidate rc;
        rc.candidateName = r.candidateName;
        rc.score = std::clamp(similarity, 0.0, 1.0);
        rc.topMatchingKeywords = topWords;
        rankings.push_back(rc);
    }

    std::sort(rankings.begin(), rankings.end(), [](const RankedCandidate& a, const RankedCandidate& b) {
        return a.score > b.score;
    });

    return rankings;
}

} // namespace resume_ranker
