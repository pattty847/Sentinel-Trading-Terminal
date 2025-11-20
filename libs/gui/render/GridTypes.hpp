#pragma once
#include <QRectF>
#include <QColor>
#include <vector>
#include <cstdint>
#include "../CoordinateSystem.h"
#include "../../core/marketdata/model/TradeData.h"

// Shared grid rendering types to avoid circular dependencies
// World-space cell; screen-space is derived in the renderer per-frame
struct CellInstance {
    // World coordinates
    int64_t timeStart_ms = 0;
    int64_t timeEnd_ms = 0;
    double priceMin = 0.0;
    double priceMax = 0.0;

    // Display/data attributes
    float liquidity = 0.0f;   // aggregated volume/liquidity
    bool isBid = true;        // side
};

struct GridSliceBatch {
    std::shared_ptr<const std::vector<CellInstance>> cells;
    std::vector<Trade> recentTrades;  // Raw trade data for bubble rendering
    double intensityScale = 1.0;
    double minVolumeFilter = 0.0;
    int maxCells = 100000;
    Viewport viewport;  // viewport snapshot for world→screen conversion
};