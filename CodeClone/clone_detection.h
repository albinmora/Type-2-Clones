#ifndef CLONEDETECTION_H
#define CLONEDETECTION_H

#include <omp.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <jsoncpp/json/json.h>

class CloneDetection{

private:

    std::string file_path;

    std::vector<std::string> method_attributes;

    Json::Value methods_array;

    int number_of_methods;

    std::vector<int> vectorizeMethod(Json::Value method);

    std::vector<int> vectorizeMethod_ThreadVersion(Json::Value method);

    int *vectorizeMethod_SIMDVersion(Json::Value method);

    std::vector<std::vector<int>> vectorizeJSON_ThreadVersion();

    int *vectorizeJSON_SIMDVersion();

public:

    CloneDetection();

    CloneDetection(std::string set_file_path);

    std::string getFilePath() const;

    void setFilePath(const std::string &value);}
    
    std::vector<std::vector<int>> vectorizeJSON();

    char compareMethods(std::vector<int> method_a,
                        std::vector<int> method_b);

    std::vector<std::vector<int>> doDetection();

    std::vector<std::vector<int>> doDetection_ThreadVersion();

    int * doDetection_SIMDVersion();

    void readDetectionResult(std::vector<std::vector<int>> result);

    void reportCodeClone(std::vector<int> detection_result);
};

#endif // CLONEDETECTION_H
