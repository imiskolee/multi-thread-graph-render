#include "multi_thread_graph_render.h"


/*
 *
 * A1 ---> B1 ---> C1 ---> D1 ---> E1
 * A2 ---> B2 ---> E1
 * A3 ---> E1
 * A4 ---> E1
 *
 * ===============
 *
 * E1 --> A4
 *
 * */


 void MultiThreadGraphRender::rebuildGraph() {
     auto connections = this->graph->getConnections();
     juce::AudioProcessorGraph::NodeAndChannel node;
     for (auto conn : connections) {
         bool isRoot = true;
         for (auto conn2 : connections) {
             if (conn.destination.nodeID == conn2.source.nodeID) {
                 isRoot = false;
                 break;
             }
         }
         if (isRoot) {
             node = conn.destination;
         }
     }

     for (auto conn : connections) {
         std::vector<juce::AudioProcessorGraph::NodeID> paths;
         if (node.nodeID == conn.destination.nodeID) {
             paths.push_back(conn.source.nodeID);
             buildNode(conn.source,&paths);
         }
         if (paths.size() > 0) {
             RenderPath renderNodes;
             renderNodes.nodes = paths;
             this->paths.push_back(renderNodes);
         }
     }
 }

void MultiThreadGraphRender::buildNode(const juce::AudioProcessorGraph::NodeAndChannel &node, std::vector<juce::AudioProcessorGraph::NodeID>* container) {
     auto connections = this->graph->getConnections();
     for (auto conn : connections) {
        if (conn.destination.nodeID == node.nodeID) {
            container->push_back(conn.source.nodeID);
            buildNode(conn.source,container);
        }
     }
}
