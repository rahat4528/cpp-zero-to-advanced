# 17-Resume-Ranker-App

## 1. Project Overview
17-Resume-Ranker-App is an automated candidate evaluation and natural language document ranking application developed in standard C++17. Designed to accelerate technical recruiting, it employs a Vector Space Information Retrieval model utilizing Term Frequency - Inverse Document Frequency (TF-IDF) weighting and Cosine Similarity metrics to match candidate resumes against target job descriptions.

## 2. Problem Statement
Manual screening of hundreds of candidate profiles is subjective and time-consuming. Human recruiters often overlook high-relevance qualifications buried in long unstructured resumes. An automated keyword and TF-IDF relevance scoring engine provides consistent, unbiased candidate qualification rankings.

## 3. Objectives
- Implement natural language text normalization: tokenization, lowercase folding, stop-word elimination, and suffix stemming.
- Build dynamic document collection vocabularies and calculate corpus-wide Document Frequencies (DF).
- Calculate Term Frequency - Inverse Document Frequency (TF-IDF) weights across multi-document corpora.
- Implement Cosine Similarity geometric vector scoring:
  $$\text{Similarity}(\mathbf{q}, \mathbf{d}) = \frac{\mathbf{q} \cdot \mathbf{d}}{\|\mathbf{q}\| \|\mathbf{d}\|}$$
- Identify and surface top matching keyword intersections for each ranked profile.
- Provide automated headless CI verification via `--test-run` and doctest unit tests.

## 4. Features
- **Deterministic Text Processing**: Cleanses raw text, strips punctuation, and extracts normalized lexical tokens.
- **Dynamic Inverse Document Frequency**: De-emphasizes terms common across all resumes while boosting distinctive technical keywords.
- **Multi-Candidate Leaderboard**: Emits sorted candidate rankings with percentage match indicators and extracted competency keywords.
- **Lightweight Zero-Dependency Design**: Native C++17 vector math without heavyweight Python/NumPy runtimes.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::string`, `std::map`, `std::unordered_set`, `std::cmath`, `std::algorithm`).
- **External Services**: None. Fully offline and self-contained.
- **Database**: None required.
- **Testing**: `doctest` unit testing framework.

## 6. Folder Structure
```
17-Resume-Ranker-App/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── TFIDFModel.hpp
│   └── TextTokenizer.hpp
├── src/
│   ├── TFIDFModel.cpp
│   ├── TextTokenizer.cpp
│   └── main.cpp
└── tests/
    └── test_resume_ranker.cpp
```

## 7. Installation / Prerequisites
- CMake 3.20 or newer.
- GCC 11+, Clang 14+, or MSVC 2022.
- doctest headers.

## 8. Build and Run Instructions
### Windows & Linux Build
```bash
cmake -S . -B build -G Ninja
cmake --build build
```

### Running the Application
```bash
./build/ResumeRanker
```

### Automated Verification Mode
```bash
./build/ResumeRanker --test-run
```

## 9. Testing Instructions
Run CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
=================================================
  AI Resume Ranker & TF-IDF Vector Space Scorer  
=================================================
Ranking 3 candidate resumes using TF-IDF Cosine Similarity...

=== CANDIDATE RANKINGS LEADERBOARD ===
 #1 David Vance (Senior C++ Engineer)          Match: 78.4%
     Top Matching Keywords: [c++] [multithread] [socket] [system] [sqlite] 
 #2 Marcus Brody (Software Developer)          Match: 24.1%
     Top Matching Keywords: [c++] [develop] [tool] 
 #3 Sarah Jenkins (Frontend Web Developer)     Match: 0.0%
     Top Matching Keywords: 
======================================
[Test Mode] TF-IDF resume ranking engine verified.
```

## 11. Known Limitations
- The built-in bag-of-words TF-IDF vectorizer evaluates lexical overlap and stems; semantic synonym embeddings (e.g. Word2Vec / BERT embeddings) require deep neural inference libraries.

## 12. Testing Status
- **Built**: Yes, with GCC 16.1.0 on Windows with 0 warnings (`-Wall -Wextra -Wpedantic`).
- **Run**: Verified TF-IDF scoring, candidate ranking leaderboard, and keyword extraction with `--test-run`.
- **Tested**: All unit test suites passed under CTest.
