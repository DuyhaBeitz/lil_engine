#pragma once

#include <vector>
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include <functional>

struct Sample {
    Vector2 position;
    float radius;
};

using RadiusFunction = std::function<float(Vector2)>;

class PoissonSampler2D {
private:

    float cell;
    int gw;
    int gh;
    std::vector<int> grid;
    std::vector<Sample> samples;
    std::vector<int> active;
    float min_radius;
    float max_radius;
    RadiusFunction radius_function;

    void Add(Sample sample) {
        int i = static_cast<int>(sample.position.x / cell);
        int j = static_cast<int>(sample.position.y / cell);
        grid[j * gw + i] = static_cast<int>(samples.size());
        samples.push_back(sample);
        active.push_back(samples.size() - 1);
    }

    float GetRandomFloat(float min, float max) {
        float d = max-min;
        float precision = 10000000;
        return float(GetRandomValue(0, precision)) / precision * d + min;
    }

    bool IsValid(Sample cand) {
        const int gi = static_cast<int>(cand.position.x / cell);
        const int gj = static_cast<int>(cand.position.y / cell);

        // The largest radius that could be relevant to this candidate
        // determines how far through the grid we need to search.
        const int range = static_cast<int>(
            std::ceil(std::max(cand.radius, max_radius) / cell)
        );

        for (int di = -range; di <= range; di++) {
            for (int dj = -range; dj <= range; dj++) {
                const int ci = gi + di;
                const int cj = gj + dj;

                if (ci < 0 || ci >= gw || cj < 0 || cj >= gh) continue;

                const int sIdx = grid[cj * gw + ci];

                if (sIdx == -1) continue;
                
                const Sample& sample = samples[sIdx];

                const float dx = sample.position.x - cand.position.x;
                const float dy = sample.position.y - cand.position.y;

                const float minDist = std::max(sample.radius, cand.radius);
                if (dx * dx + dy * dy < minDist * minDist) return false;
            }
        }
        return true;
    }

public:
    std::vector<Sample> PoissonDisk(float width, float height, float min_rad, float max_rad, RadiusFunction radius_at, int k = 30) {
        min_radius = min_rad;
        max_radius = max_rad;
        radius_function = radius_at;

        cell = min_radius / std::sqrt(2.0f);

        gw = std::ceil(width / cell);
        gh = std::ceil(height / cell);

        grid = std::vector<int>(gw*gh, -1);


        Vector2 start = {
            width / 2.0f,
            height / 2.0f
        };
        float startRadius = radius_function(start);
        Add(Sample{start, startRadius});

        while (!active.empty()) {
            int ri = GetRandomValue(0, active.size()-1);
            const Sample& parent = samples[active[ri]];
            bool found = false;

            for (int n = 0; n < k; n++) {
                float angle = GetRandomFloat(0.0f, 1.0f) * PI * 2;
                float dist  = parent.radius + GetRandomFloat(0.0f, 1.0f) * parent.radius;
                
                Vector2 cand = parent.position + Vector2{dist * std::cos(angle), dist * std::sin(angle)};

                if (cand.x < 0 || cand.x >= width ||
                    cand.y < 0 || cand.y >= height) continue;

                float candidateRadius = radius_function(cand);

                if (candidateRadius < min_radius ||
                    candidateRadius > max_radius)
                    continue;

                if (!IsValid(Sample{cand, candidateRadius})) continue;

                Add({cand, candidateRadius});
                found = true;
                break;
            }
            if (!found) {
                active[ri] = active.back();
                active.pop_back();
            }
        }
        return samples;
    }
};