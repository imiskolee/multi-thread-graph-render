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
     this->root = new Node();
     this->root->current = this->graph->getNodeForId(node.nodeID)->getProcessor();
     this->buildNode(node,this->root);
 }

Node* MultiThreadGraphRender::buildNode(const juce::AudioProcessorGraph::NodeAndChannel &node,Node* container) {
     auto connections = this->graph->getConnections();
     for (auto conn : connections) {
        if (conn.destination.nodeID == node.nodeID) {
            auto n = new Node();
            n->current = this->graph->getNodeForId(conn.source.nodeID)->getProcessor();
            buildNode(conn.source,n);
            container->children.push_back(n);
        }
     }
     return container;
}
