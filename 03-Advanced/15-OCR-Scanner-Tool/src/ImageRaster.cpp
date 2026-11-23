#include "ImageRaster.hpp"
#include <queue>
#include <algorithm>
#include <cmath>

namespace ocr {

ImageRaster::ImageRaster(int width, int height, uint8_t initialColor)
    : m_width(width), m_height(height), m_pixels(static_cast<size_t>(width * height), initialColor) {}

uint8_t ImageRaster::getPixel(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) return 255;
    return m_pixels[static_cast<size_t>(y * m_width + x)];
}

void ImageRaster::setPixel(int x, int y, uint8_t val) {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        m_pixels[static_cast<size_t>(y * m_width + x)] = val;
    }
}

void ImageRaster::drawRect(int x, int y, int w, int h, uint8_t color, bool filled) {
    for (int cy = y; cy < y + h; ++cy) {
        for (int cx = x; cx < x + w; ++cx) {
            if (filled || cy == y || cy == y + h - 1 || cx == x || cx == x + w - 1) {
                setPixel(cx, cy, color);
            }
        }
    }
}

void ImageRaster::drawGlyph8x8(int startX, int startY, const uint8_t glyph[8], uint8_t fgColor) {
    for (int r = 0; r < 8; ++r) {
        uint8_t rowBits = glyph[r];
        for (int c = 0; c < 8; ++c) {
            if ((rowBits >> (7 - c)) & 1) {
                setPixel(startX + c, startY + r, fgColor);
            }
        }
    }
}

ImageRaster ImageRaster::binarizeOtsu() const {
    if (m_pixels.empty()) return *this;

    // Histogram of 256 grayscale levels
    std::vector<int> hist(256, 0);
    for (uint8_t p : m_pixels) hist[p]++;

    int total = m_width * m_height;
    double sum = 0.0;
    for (int t = 0; t < 256; ++t) sum += t * hist[t];

    double sumB = 0.0;
    int wB = 0;
    double maxVar = 0.0;
    int threshold = 128;

    for (int t = 0; t < 256; ++t) {
        wB += hist[t];
        if (wB == 0) continue;
        int wF = total - wB;
        if (wF == 0) break;

        sumB += static_cast<double>(t * hist[t]);
        double mB = sumB / wB;
        double mF = (sum - sumB) / wF;

        double betweenVar = static_cast<double>(wB) * static_cast<double>(wF) * (mB - mF) * (mB - mF);
        if (betweenVar > maxVar) {
            maxVar = betweenVar;
            threshold = t;
        }
    }

    ImageRaster out(m_width, m_height, 255);
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            out.setPixel(x, y, getPixel(x, y) <= threshold ? 0 : 255);
        }
    }
    return out;
}

std::vector<BoundingBox> ImageRaster::segmentConnectedComponents() const {
    std::vector<BoundingBox> boxes;
    std::vector<bool> visited(static_cast<size_t>(m_width * m_height), false);

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            size_t idx = static_cast<size_t>(y * m_width + x);
            // Looking for foreground dark pixels (< 128)
            if (m_pixels[idx] < 128 && !visited[idx]) {
                // BFS flood-fill
                std::queue<std::pair<int, int>> q;
                q.push({x, y});
                visited[idx] = true;

                int minX = x, maxX = x, minY = y, maxY = y;
                int pixelCount = 0;

                while (!q.empty()) {
                    auto [cx, cy] = q.front();
                    q.pop();
                    pixelCount++;

                    minX = std::min(minX, cx);
                    maxX = std::max(maxX, cx);
                    minY = std::min(minY, cy);
                    maxY = std::max(maxY, cy);

                    const int dx[4] = {0, 0, 1, -1};
                    const int dy[4] = {1, -1, 0, 0};

                    for (int k = 0; k < 4; ++k) {
                        int nx = cx + dx[k];
                        int ny = cy + dy[k];
                        if (nx >= 0 && nx < m_width && ny >= 0 && ny < m_height) {
                            size_t nIdx = static_cast<size_t>(ny * m_width + nx);
                            if (m_pixels[nIdx] < 128 && !visited[nIdx]) {
                                visited[nIdx] = true;
                                q.push({nx, ny});
                            }
                        }
                    }
                }

                int bw = maxX - minX + 1;
                int bh = maxY - minY + 1;
                // Filter single-pixel noise or massive background
                if (bw >= 2 && bh >= 3 && pixelCount >= 4) {
                    boxes.push_back({minX, minY, bw, bh});
                }
            }
        }
    }
    return boxes;
}

std::vector<uint8_t> ImageRaster::extractNormalized8x8(const BoundingBox& box) const {
    std::vector<uint8_t> norm(64, 0); // 8x8 flattened

    for (int ny = 0; ny < 8; ++ny) {
        for (int nx = 0; nx < 8; ++nx) {
            // Nearest-neighbor / area mapping from box to 8x8
            int srcX = box.x + (nx * box.width) / 8;
            int srcY = box.y + (ny * box.height) / 8;
            norm[static_cast<size_t>(ny * 8 + nx)] = (getPixel(srcX, srcY) < 128) ? 1 : 0;
        }
    }
    return norm;
}

} // namespace ocr
