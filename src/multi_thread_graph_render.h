#ifndef MULTI_THREAD_GRAPH_RENDER_H
#define MULTI_THREAD_GRAPH_RENDER_H
#include <juce_audio_processors/juce_audio_processors.h>
#include "wait_group.h"

struct RenderPath {
    std::vector<juce::AudioProcessorGraph::NodeID> nodes;
};

class RenderJob : public juce::ThreadPoolJob {
public:
    RenderJob(juce::ThreadPool *pool, WaitGroup* wg,juce::AudioProcessorGraph* graph,RenderPath path,juce::AudioProcessorGraph::NodeID nodeID,juce::AudioBuffer<float>* buffer,juce::MidiBuffer* midiMessages): ThreadPoolJob("RenderJob") {
        this->pool = pool;
        this->path = path;
        this->graph = graph;
        this->midiMessages = midiMessages;
        this->buffer = buffer;
        this->nodeID = nodeID;
        this->wg = wg;
    }
    JobStatus runJob() override {
        auto node = this->graph->getNodeForId(nodeID);
        node->getProcessor()->processBlock(*buffer,*midiMessages);
        this->wg->done();
        for (int i=0;i < path.nodes.size();i++) {
            if (nodeID == path.nodes[i] && i != 0) {
                auto job = new RenderJob(
                    this->pool,
                    this->wg,
                    this->graph,
                    this->path,
                    path.nodes[i-1],
                    buffer,
                    midiMessages
                    );
                this->pool->addJob(job,true);
            }
        }
        return JobStatus::jobHasFinished;
    }

private:
    juce::ThreadPool* pool;
    juce::AudioBuffer<float>* buffer;
    juce::MidiBuffer* midiMessages;
    juce::AudioProcessorGraph* graph;
    RenderPath path;
    juce::AudioProcessorGraph::NodeID nodeID;
    WaitGroup* wg;
};

class MultiThreadGraphRender {
public:
    MultiThreadGraphRender(juce::AudioProcessorGraph* graph, juce::AudioProcessorGraph::NodeID rootNodeID,int numOfThreads = 4): graph(graph),rootNodeID(rootNodeID) {
        threadPool.reset(new juce::ThreadPool(numOfThreads));
        this->rebuildGraph();
    }
    void debug() {
        for (auto p : this->paths) {
            printNode(&p);
        }
    }
    void process(juce::AudioBuffer<float>& buffer,  juce::MidiBuffer& midiMessages) {
        int totalJobs = 0;
        WaitGroup wg;
        for (auto p : this->paths) {
            totalJobs += p.nodes.size();
        }
        wg.add(totalJobs);
        for (auto p : this->paths) {
            auto start = p.nodes[p.nodes.size() -1];
             auto j = new RenderJob(
                this->threadPool.get(),
                &wg,
                this->graph,
                p,
                start,
                &buffer,
                &midiMessages
            );
            this->threadPool->addJob(j,true);
        }
        wg.wait();
    }

    void printNode(const RenderPath* path) {
        for (auto child : path->nodes) {
            auto node = this->graph->getNodeForId(child);
            std::cout << " " << node->getProcessor()->getName() << " ->";
        }
        std::cout << "\n" << std::endl;
    }
private:
    void rebuildGraph();
    void buildNode(const juce::AudioProcessorGraph::NodeAndChannel& node,std::vector<juce::AudioProcessorGraph::NodeID>* container) ;

    std::unique_ptr<juce::ThreadPool> threadPool;
    juce::AudioProcessorGraph* graph = nullptr;
    juce::AudioProcessorGraph::NodeID rootNodeID;
    std::vector<RenderPath> paths;

};


#endif //MULTI_THREAD_GRAPH_RENDER_H
