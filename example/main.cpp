#include <juce_audio_processors/juce_audio_processors.h>

#include "../src/multi_thread_graph_render.h"
#include <random>
class ExampleProcessor : public juce::AudioProcessor {
private:
    juce::String name;
    public:
    ExampleProcessor(juce::String name) : name(name), AudioProcessor (BusesProperties()
                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)) {

        }
    const juce::String getName() const override {
            return name;
        }

    void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override {}

    void releaseResources() override {}

    void processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) override {
        std::cout << "Processing " << this->getName() << std::endl;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1000, 5000);
        std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
        std::cout << "Processed " << this->getName() << std::endl;
    }

    double getTailLengthSeconds() const override { return 0.0; }

    bool acceptsMidi() const override { return true;}

    bool producesMidi() const override {return true;}

    juce::AudioProcessorEditor * createEditor() override { return nullptr;}

    bool hasEditor() const override {return false;}

    int getNumPrograms() override {return 1;}

    int getCurrentProgram() override {return 1;}

    void setCurrentProgram(int index) override {}

    const juce::String getProgramName(int index) override {
            return name;
        }

    void changeProgramName(int index, const juce::String &newName) override {

        }

    void getStateInformation(juce::MemoryBlock &destData) override {

        }

    void setStateInformation(const void *data, int sizeInBytes) override {

        }


};


void make_connection(juce::AudioProcessorGraph* graph,const juce::AudioProcessorGraph::Node::Ptr& first,const juce::AudioProcessorGraph::Node::Ptr& second) {
    juce::AudioProcessorGraph::Connection connection;
    connection.source = {first->nodeID, 0};
    connection.destination = {second->nodeID, 0};
    auto success = graph->addConnection(connection,juce::AudioProcessorGraph::UpdateKind::sync);

}


int main() {
    juce::AudioProcessorGraph *graph = new juce::AudioProcessorGraph();

   auto root = std::make_unique<ExampleProcessor>("Master Mixer");
    auto rootNode =  graph->addNode(std::move(root));
    {
        std::unique_ptr<juce::AudioProcessor> t1 = std::make_unique<ExampleProcessor>("Track 1");
        auto p1 = std::make_unique<ExampleProcessor>("EQ 1");
        auto p2 = std::make_unique<ExampleProcessor>("Limiter 1");
        auto n1 = graph->addNode(std::move(t1),{},juce::AudioProcessorGraph::UpdateKind::sync);
        auto n2 = graph->addNode(std::move(p1),{},juce::AudioProcessorGraph::UpdateKind::sync);
        auto n3 = graph->addNode(std::move(p2),{},juce::AudioProcessorGraph::UpdateKind::sync);
        {
            make_connection(graph,n1,n2);
            make_connection(graph,n2,n3);
            make_connection(graph,n3,rootNode);
        }

    }
    {
        std::unique_ptr<juce::AudioProcessor> t1 = std::make_unique<ExampleProcessor>("Track 2");
        auto p1 = std::make_unique<ExampleProcessor>("EQ 2");
        auto p2 = std::make_unique<ExampleProcessor>("Limiter 2");
        auto p3 = std::make_unique<ExampleProcessor>("Reverb 2");
        auto n1 = graph->addNode(std::move(t1),{},juce::AudioProcessorGraph::UpdateKind::sync);
        auto n2 = graph->addNode(std::move(p1),{},juce::AudioProcessorGraph::UpdateKind::sync);
        auto n3 = graph->addNode(std::move(p2),{},juce::AudioProcessorGraph::UpdateKind::sync);
        auto n4 = graph->addNode(std::move(p3),{},juce::AudioProcessorGraph::UpdateKind::sync);
        {
            make_connection(graph,n1,n2);
            make_connection(graph,n2,n3);
            make_connection(graph,n3,n4);
            make_connection(graph,n4,rootNode);
        }
    }

    {
        std::unique_ptr<juce::AudioProcessor> t1 = std::make_unique<ExampleProcessor>("Track 3");
        auto p1 = std::make_unique<ExampleProcessor>("EQ 3");
        auto n1 = graph->addNode(std::move(t1),{},juce::AudioProcessorGraph::UpdateKind::sync);
        auto n2 = graph->addNode(std::move(p1),{},juce::AudioProcessorGraph::UpdateKind::sync);
        {
            make_connection(graph,n1,n2);
            make_connection(graph,n2,rootNode);
        }
    }
    auto render = new MultiThreadGraphRender(graph,rootNode->nodeID);
    render->debug();
    juce::AudioBuffer<float> audioBuffer;
    juce::MidiBuffer midiBuffer;
    render->process(audioBuffer,midiBuffer);
}