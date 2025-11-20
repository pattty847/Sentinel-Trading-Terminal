/*
Sentinel — GridSceneNode
Role: Implements the logic for managing the chart's scene graph structure.
Inputs/Outputs: Handles the creation and deletion of child nodes as the chart updates.
Threading: All code is executed on the Qt Quick render thread.
Performance: The destructor ensures proper memory management of scene graph nodes.
Integration: The concrete implementation of the chart's root scene graph node.
Observability: No internal logging.
Related: GridSceneNode.hpp.
Assumptions: The active render strategy returns a valid node to be added to the scene.
*/
#include "GridSceneNode.hpp"
#include "GridTypes.hpp"
#include "IRenderStrategy.hpp"
#include "IDataAccessor.hpp"
#include <QSGGeometryNode>
#include <QSGVertexColorMaterial>
#include <QSGGeometry>

GridSceneNode::GridSceneNode() {
    setFlag(QSGNode::OwnedByParent);
}

void GridSceneNode::updateLayeredContent(const IDataAccessor* dataAccessor, 
                                        IRenderStrategy* heatmapStrategy, bool showHeatmap,
                                        IRenderStrategy* bubbleStrategy, bool showBubbles,
                                        IRenderStrategy* flowStrategy, bool showFlow) {
    
    // Update heatmap layer
    if (showHeatmap && heatmapStrategy) {
        if (m_heatmapNode) {
            removeChildNode(m_heatmapNode);
            delete m_heatmapNode;
        }
        m_heatmapNode = heatmapStrategy->buildNode(dataAccessor);
        if (m_heatmapNode) {
            appendChildNode(m_heatmapNode);
        }
    } else if (m_heatmapNode) {
        removeChildNode(m_heatmapNode);
        delete m_heatmapNode;
        m_heatmapNode = nullptr;
    }
    
    // Update bubble layer
    if (showBubbles && bubbleStrategy) {
        if (m_bubbleNode) {
            removeChildNode(m_bubbleNode);
            delete m_bubbleNode;
        }
        m_bubbleNode = bubbleStrategy->buildNode(dataAccessor);
        if (m_bubbleNode) {
            appendChildNode(m_bubbleNode);
        }
    } else if (m_bubbleNode) {
        removeChildNode(m_bubbleNode);
        delete m_bubbleNode;
        m_bubbleNode = nullptr;
    }
    
    // Update flow layer  
    if (showFlow && flowStrategy) {
        if (m_flowNode) {
            removeChildNode(m_flowNode);
            delete m_flowNode;
        }
        m_flowNode = flowStrategy->buildNode(dataAccessor);
        if (m_flowNode) {
            appendChildNode(m_flowNode);
        }
    } else if (m_flowNode) {
        removeChildNode(m_flowNode);
        delete m_flowNode;
        m_flowNode = nullptr;
    }
}

void GridSceneNode::updateTransform(const QMatrix4x4& transform) {
    setMatrix(transform);
    markDirty(QSGNode::DirtyMatrix);
}

void GridSceneNode::setShowVolumeProfile(bool show) {
    if (m_showVolumeProfile != show) {
        m_showVolumeProfile = show;
        
        if (m_volumeProfileNode) {
            m_volumeProfileNode->setFlag(QSGNode::OwnedByParent, false);
            removeChildNode(m_volumeProfileNode);
            
            if (!show) {
                delete m_volumeProfileNode;
                m_volumeProfileNode = nullptr;
            } else {
                appendChildNode(m_volumeProfileNode);
                m_volumeProfileNode->setFlag(QSGNode::OwnedByParent, true);
            }
        }
    }
}

void GridSceneNode::updateVolumeProfile(const std::vector<std::pair<double, double>>& profile) {
    if (!m_showVolumeProfile) return;
    
    // Remove old volume profile node
    if (m_volumeProfileNode) {
        removeChildNode(m_volumeProfileNode);
        delete m_volumeProfileNode;
        m_volumeProfileNode = nullptr;
    }
    
    m_volumeProfileNode = createVolumeProfileNode(profile);
    
    if (m_volumeProfileNode) {
        appendChildNode(m_volumeProfileNode);
        m_volumeProfileNode->setFlag(QSGNode::OwnedByParent, true);
    }
}

QSGNode* GridSceneNode::createVolumeProfileNode(const std::vector<std::pair<double, double>>& profile) {
    // Placeholder for volume profile implementation
    // In a real implementation, this would create geometry based on the profile data
    return nullptr;
}
