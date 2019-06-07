#include <iostream>
#include "clone_detection.h"

using namespace std;


void generarWorkload(int cantidad, std::string archivo){

    ofstream ofs;

    // Se abre o crea el archivo
    ofs.open(archivo);

    // JSON Object
    Json::Value writeValue;

    // JSON Array
    Json::Value array;

    // Objeto para escribir el JSON en el archivo
    Json::StyledWriter sw;

    // Se agregan atributos al JSON Object
    writeValue["LN"] = 10;
    writeValue["IF"] = 1;
    writeValue["CIC"] = 1;
    writeValue["RTN"] = 1;
    writeValue["ARG"] = 2;
    writeValue["LOC"] = 1;
    writeValue["FUN"] = 3;
    writeValue["class"] = "Logger";
    writeValue["name"] = "foo";

    for(int i = 0; i < cantidad; ++i){

        // Se agrega al JSON Array
        array.append(writeValue);

    }

    // Escribo el JSON Array al archivo
    ofs << sw.write(array);

    // Se cierra el archivo
    ofs.close();
}

int main(){

    //generarWorkload(10000,"/home/alfredo/Documentos/TEC/Arqui_II/Proyecto Final/Type-2-Clones/prueba10k.json");

    string prueba = "/home/alfredo/Documentos/TEC/Arqui_II/Proyecto Final/Type-2-Clones/prueba10k.json";

    CloneDetection cd(prueba);

    double start_time, run_time;

    start_time = omp_get_wtime();
    //cd.readDetectionResult(cd.doDetection());
    cd.doDetection();
    run_time = omp_get_wtime() - start_time;

    cout << "Serial code clone detection in " << run_time << " seconds" << endl << endl;

    start_time = omp_get_wtime();
    //cd.readDetectionResult(cd.doDetection_ThreadVersion());
    cd.doDetection_ThreadVersion();
    run_time = omp_get_wtime() - start_time;

    cout << "OpenMP code clone detection in " << run_time << " seconds" << endl;

    return 0;
}

