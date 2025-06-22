#ifndef MULTI_THREAD_GRAPH_RENDER_H
#define MULTI_THREAD_GRAPH_RENDER_H
#include <juce_audio_processors/juce_audio_processors.h>

struct Node {
    juce::AudioProcessor* current;
    std::vector<Node*> children;
};


class MultiThreadGraphRender {
public:
    MultiThreadGraphRender(juce::AudioProcessorGraph* graph): graph(graph) {

    }
    void debug() {
        this->rebuildGraph();
        this->printNode(this->root,"");
    }
    void printNode(const Node* node, const std::string &prefix) {
        std::cout << prefix << node->current->getName() << std::endl;
        for (auto child : node->children) {
            printNode(child,prefix + "  ");
        }
    }
private:
    void rebuildGraph();
    Node* buildNode(const juce::AudioProcessorGraph::NodeAndChannel& node,Node* container) ;
juce::AudioProcessorGraph* graph = nullptr;
    Node* root = nullptr;
};


#endif //MULTI_THREAD_GRAPH_RENDER_H
