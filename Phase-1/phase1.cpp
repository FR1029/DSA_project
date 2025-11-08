#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
/*
    Add other includes that you require, only write code wherever indicated
*/
#include "phase1-utils.hpp"

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <graph.json> <queries.json> <output.json>" << std::endl;
        return 1;
    }

    // Read graph from first file
    std::ifstream graph_file(argv[1]);
    if (!graph_file.is_open()) {
        std::cerr << "Failed to open " << argv[1] << std::endl;
        return 1;
    }

    json graph_json;
    try {
        graph_file >> graph_json;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return 1;
    }

    graph_file.close();
    
    Graph Graph_Map;
    try {
        Graph_Map.graph_load_json(graph_json);
    }
    catch (const std::exception& e) {
        std::cerr << "Error constructing graph: " << e.what() << std::endl;
    }

    /*
        Add your graph reading and processing code here
        Initialize any classes and data structures needed for query processing
        Close the file after reading it
    */

    // Read queries from second file
    std::ifstream queries_file(argv[2]);
    if (!queries_file.is_open()) {
        std::cerr << "Failed to open " << argv[2] << std::endl;
        return 1;
    }

    json queries_json;
    queries_file >> queries_json;
    queries_file.close();

    json meta = queries_json["meta"];
    std::vector<json> results;

    for (const auto& query : queries_json["events"]) {
        auto start_time = std::chrono::high_resolution_clock::now();

        /*
            Add your query processing code here
            Each query should return a json object which should be printed to sample.json
        */
        json result;
        try {
            result = Graph_Map.process_query(query);
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to open " << argv[2] << std::endl;
            return 1;
        }

        // Answer each query replacing the function process_query using 
        // whatever function or class methods that you have implemented
        

        auto end_time = std::chrono::high_resolution_clock::now();
        result["processing_time"] = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        results.push_back(result);
    }

    std::ofstream output_file(argv[3]);
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output.json for writing" << std::endl;
        return 1;
    }

    json output;
    output["meta"] = meta;
    output["results"] = results;
    output_file << output.dump(4) << std::endl;

    output_file.close();
    return 0;
}