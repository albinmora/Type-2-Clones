#include "clone_detection.h"
#include  <xmmintrin.h>
#include  <emmintrin.h>

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

    vectorize_method.resize(method_attributes.size());

    for(int i = 0; i < (int) method_attributes.size(); ++i){

        vectorize_method[i] = method[method_attributes[i]].asInt();
    }

    return vectorize_method;

}

int * CloneDetection::vectorizeMethod_SIMDVersion(Json::Value method){

    int *vectorize_method;

    vectorize_method = (int *)malloc(sizeof(int) * method_attributes.size() + 1);

    for(int i = 0; i < (int) method_attributes.size(); ++i){

        vectorize_method[i] = method[method_attributes[i]].asInt();
    }
    vectorize_method[method_attributes.size()] = 0;
    return vectorize_method;

}

std::vector<int> CloneDetection::vectorizeMethod_ThreadVersion(Json::Value method){

    std::vector<int> vectorize_method;

    vectorize_method.resize(method_attributes.size());

    omp_set_num_threads(2*omp_get_num_procs());

    #pragma omp parallel
    {
        #pragma omp for
        for(int i = 0; i < (int) method_attributes.size(); ++i){

            vectorize_method[i] = method[method_attributes[i]].asInt();
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

int * CloneDetection::vectorizeJSON_SIMDVersion(){

    int * vectors;
    int * sub_vec;

    // Abrir el archivo
    std::ifstream ifs(file_path);

    // Objeto para leer el JSON
    Json::Reader reader;

    // Objeto para guardar un JSON Object leido
    Json::Value array;

    // Leer todo el JSON Array o JSON Object que este en el archivo
    reader.parse(ifs, array);

    vectors = (int *)malloc(sizeof(int) * 8 * array.size());

    number_of_methods = array.size();

    for(int i = 0; i < (int)array.size(); ++i){
    	sub_vec = vectorizeMethod_SIMDVersion(array[i]);
    	for(int j = 0; j < 8; j++){
    		 vectors[i * 8 + j] = sub_vec[j];
    	}
    }

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

    vectors.resize(array.size());

    omp_set_num_threads(2*omp_get_num_procs());

    #pragma omp parallel
    {
        #pragma omp for
        for(int i = 0; i < (int)array.size(); ++i){

           vectors[i] = vectorizeMethod_ThreadVersion(array[i]);
        }
    }

    methods_array = array;

    ifs.close();

    return vectors;
}

std::vector<std::vector<int>> CloneDetection::doDetection(){

    int i, j;
    std::vector<std::vector<int>> vector_array;
    std::vector<std::vector<int>> result_array;

    vector_array = vectorizeJSON();

    for(i = 0; i < (int) vector_array.size(); ++i){

        result_array.push_back(std::vector<int>());
    }

    for(i = 0; i < (int) vector_array.size(); ++i){

        for(j = i+1; j < (int) vector_array.size(); ++j){

            if(vector_array[i][0] == vector_array[j][0] &&
               vector_array[i][1] == vector_array[j][1] &&
               vector_array[i][2] == vector_array[j][2] &&
               vector_array[i][3] == vector_array[j][3] &&
               vector_array[i][4] == vector_array[j][4] &&
               vector_array[i][5] == vector_array[j][5] &&
               vector_array[i][6] == vector_array[j][6]){

                result_array[i].push_back(j);

            }
        }
    }

    return result_array;
}

int * CloneDetection::doDetection_SIMDVersion(){
    int *vector_array;
    int *result_array;
    __m128i vector_arrayP1;
    __m128i vector_arrayP2;
    __m128i vector_arrayS1;
    __m128i vector_arrayS2;

    __m128i res1;
    __m128i res2;

    int eq1 = 0;
    int eq2 = 0;

    vector_array = vectorizeJSON_SIMDVersion();
    result_array = (int *)calloc(sizeof(int), 8 * number_of_methods);


    for(int i = 0; i < number_of_methods; ++i){
        for(int j = i+1; j < number_of_methods; ++j){
        	vector_arrayP1 = _mm_load_si128((__m128i*)(vector_array + (i*4)));
        	vector_arrayP2 = _mm_load_si128((__m128i*)(vector_array + (i*4) + 4));

        	vector_arrayS1 = _mm_load_si128((__m128i*)(vector_array + (j*4)));
        	vector_arrayS2 = _mm_load_si128((__m128i*)(vector_array + (j*4) + 4));

        	res1  =_mm_cmpeq_epi32 (vector_arrayP1, vector_arrayS1);
        	res2  =_mm_cmpeq_epi32 (vector_arrayP2, vector_arrayS2);

        	_mm_store_si128((__m128i*)&eq1, res1);
        	_mm_store_si128((__m128i*)&eq2, res2);
        	if(eq1 != 0 && eq2 !=0) result_array[i] =  j;
        }
    }
    return result_array;
}

std::vector<std::vector<int>> CloneDetection::doDetection_ThreadVersion(){

    std::vector<std::vector<int>> vector_array;
    std::vector<std::vector<int>> result_array;

    vector_array = vectorizeJSON_ThreadVersion();

    for(int k = 0; k < (int) vector_array.size(); ++k){

        result_array.push_back(std::vector<int>());
    }

    omp_set_num_threads(2*omp_get_num_procs());

    #pragma omp parallel
    {
        #pragma omp for
        for(int i = 0; i < (int) vector_array.size(); ++i){

            for(int j = i+1; j < (int) vector_array.size(); ++j){

                if(vector_array[i][0] == vector_array[j][0] &&
                   vector_array[i][1] == vector_array[j][1] &&
                   vector_array[i][2] == vector_array[j][2] &&
                   vector_array[i][3] == vector_array[j][3] &&
                   vector_array[i][4] == vector_array[j][4] &&
                   vector_array[i][5] == vector_array[j][5] &&
                   vector_array[i][6] == vector_array[j][6]){


                    result_array[i].push_back(j);

                }
            }
        }
    }

    return result_array;
}

void CloneDetection::readDetectionResult(std::vector<std::vector<int>> result){

    int i,j,clone_id;

    std::vector<int> flags;

    flags.resize(result.size());

    for(i = 0; i < (int) result.size(); ++i){

        if(result[i].size() > 0 && !flags[i]){

            std::cout << "Analyzed => Class: " << methods_array[i]["class"] << " Method: " << methods_array[i]["name"] << std::endl;

            for(j = 0; j < (int) result[i].size(); ++j){

                clone_id = result[i][j];

                if(!flags[clone_id]){

                    std::cout << "Clone detected => Class: " << methods_array[clone_id]["class"] << " Method: " << methods_array[clone_id]["name"] << std::endl;
                    flags[clone_id] = 1;
                }
            }
        }
    }
}

