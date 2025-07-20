# multi-thread-graph-render
A Multi Threads JUCE Audio Processor Graph Render For Realtime &amp; Offline 


The original design is based on an Audio Graph with the following structure:
```sh
MasterNode:
    Track 1:
        Plugin 1
        Plugin 2
    Track 2:
        Plugin 1
        Plugin 2
        Plugin N
```

Given such constraints, it is possible to derive the dependency chain in reverse from the connection information of the Audio Graph, thereby enabling automatically adjusted parallel processing using the dependency order.


## Usage

```cpp
 juce::AudioProcessorGraph *graph = new juce::AudioProcessorGraph();
 
 //make AudioProcessor connections as your logics
 
 auto render = new MultiThreadGraphRender(graph,rootNode->nodeID,numOfThrads);
 render->debug();
 juce::AudioBuffer<float> audioBuffer;
 juce::MidiBuffer midiBuffer;
 render->process(audioBuffer,midiBuffer);
```