#include "clone_detection.h"


CloneDetection::CloneDetection(){

    method_attributes.resize(7);

    method_attributes[0] = "LN";
    method_attributes[1] = "IF";
    method_attributes[2] = "CIC";
    method_attributes[3] = "RTN";
    method_attributes[4] = "ARG";
    method_attributes[5] = "LOC";
    method_attributes[6] = "FUN";

}

CloneDetection::CloneDetection(std::__cxx11::string set_file_path){

    file_path = set_file_path;

    method_attributes.resize(7);

    method_attributes[0] = "LN";
    method_attributes[1] = "IF";
    method_attributes[2] = "CIC";
    method_attributes[3] = "RTN";
    method_attributes[4] = "ARG";
    method_attributes[5] = "LOC";
    method_attributes[6] = "FUN";
}

std::string CloneDetection::getFilePath() const{

    return file_path;
}

void CloneDetection::setFilePath(const std::string &value){

    file_path = value;

}

std::vector<int> CloneDetection::vectorizeMethod(Json::Value method){

    std::vector<int> vectorize_method;

    for(int i = 0; i < (int) method_attributes.size(); ++i){

        vectorize_method.push_back(method[method_attributes[i]].asInt());
    }

    return vectorize_method;

}

std::vector<int> CloneDetection::vectorizeMethod_ThreadVersion(Json::Value method){

    std::vector<int> vectorize_method;

    omp_set_num_threads(2*omp_get_num_procs());

    #pragma omp parallel
    {
        #pragma omp for
        for(int i = 0; i < (int) method_attributes.size(); ++i){

            vectorize_method.push_back(method[method_attributes[i]].asInt());
        }
    }

    return vectorize_method;

}

std::vector<std::vector<int>> CloneDetection::vectorizeJSON(){

    std::vector<std::vector<int>> vectors;

    // Abrir el archivo
    std::ifstream ifs(file_path);

    // Objeto para leer el JSON
    Json::Reader reader;

    // Objeto para guardar un JSON Object leido
    Json::Value array;

    // Leer todo el JSON Array o JSON Object que este en el archivo
    reader.parse(ifs, array);

    for(int i = 0; i < (int)array.size(); ++i){

       vectors.push_back(vectorizeMethod(array[i]));
    }

    methods_array = array;

    ifs.close();

    return vectors;
}

std::vector<std::vector<int>> CloneDetection::vectorizeJSON_ThreadVersion(){

    std::vector<std::vector<int>> vectors;

    // Abrir el archivo
    std::ifstream ifs(file_path);

    // Objeto para leer el JSON
    Json::Reader reader;

    // Objeto para guardar un JSON Object leido
    Json::Value array;

    // Leer todo el JSON Array o JSON Object que este en el archivo
    reader.parse(ifs, array);

    omp_set_num_threads(2*omp_get_num_procs());

    #pragma omp parallel
    {
        #pragma omp for
        for(int i = 0; i < (int)array.size(); ++i){

           vectors.push_back(vectorizeMethod_ThreadVersion(array[i]));
        }
    }

    methods_array = array;

    ifs.close();

    return vectors;
}

std::vector<int> CloneDetection::doDetection(){

    int clone_id, i, j;
    std::vector<std::vector<int>> vector_array;
    std::vector<int> result;

    vector_array = vectorizeJSON();

    result.resize(vector_array.size());

    clone_id = 1;

    for(i = 0; i < (int) vector_array.size(); ++i){

        if(result[i] == 0){

            result[i] = clone_id;
            ++clone_id;
        }

        for(j = i+1; j < (int) vector_array.size(); ++j){

            if(vector_array[i] == vector_array[j]){

                if(result[j] == 0){

                    result[j] = clone_id - 1;
                }
            }
        }
    }

    return result;
}

std::vector<int> CloneDetection::doDetection_ThreadVersion(){

    int clone_id, i, j;
    std::vector<std::vector<int>> vector_array;
    std::vector<int> result;

    vector_array = vectorizeJSON_ThreadVersion();

    result.resize(vector_array.size());

    clone_id = 1;

    omp_set_num_threads(2*omp_get_num_procs());

    #pragma omp parallel
    {
        #pragma omp for
        for(i = 0; i < (int) vector_array.size(); ++i){

            if(result[i] == 0){

                result[i] = clone_id;
                ++clone_id;
            }

            #pragma omp for
            for(j = i+1; j < (int) vector_array.size(); ++j){

                if(vector_array[i] == vector_array[j]){

                    if(result[j] == 0){

                        result[j] = clone_id - 1;
                    }
                }
            }
        }
    }

    return result;

}

void CloneDetection::reportCodeClone(std::vector<int> detection_result){

    int i,j;
    char clone_detected;

    for(i = 0; i < (int) detection_result.size(); ++i){

        clone_detected = 0;

        for(j = i+1; j < (int) detection_result.size(); ++j){

            if(detection_result[j] != 0 &&
               detection_result[i] == detection_result[j]){

                if(!clone_detected){

                    std::cout << "Analyzing => Class: " << methods_array[i]["class"] << " Method: " << methods_array[i]["name"] << std::endl;
                    clone_detected = 1;
                }

                std::cout << "Clone detected => Class: " << methods_array[j]["class"] << " Method: " << methods_array[j]["name"] << std::endl;

                detection_result[j] = 0;
            }
        }

        if(clone_detected){

            std::cout << std::endl;
        }
    }
}

