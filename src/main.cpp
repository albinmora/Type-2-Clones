#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <../include/json_parser.hpp>

using namespace std;

void ejemplo(){

    // ################################################################################################
    //                      Proceso de escritura
    // ################################################################################################

    ofstream ofs;

    // Se abre o crea el archivo
    ofs.open("/home/albin/Documents/creado.json");

    // JSON Object
    Json::Value writeValue;

    // JSON Array
    Json::Value array;

    // Objeto para escribir el JSON en el archivo
    Json::StyledWriter sw;

    // Se agregan atributos al JSON Object
    writeValue["language"] = string("Java");
    writeValue["id"] = 1;

    // Se agrega al JSON Array
    array.append(writeValue);

    // Genero otro JSON Object
    writeValue["language"] = string("Python");
    writeValue["id"] = 2;

    // Se agrega al JSON Array
    array.append(writeValue);

    // Escribo el JSON Array al archivo
    ofs << sw.write(array);

    // Se cierra el archivo
    ofs.close();

    // ################################################################################################
    //                      Proceso de lectura
    // ################################################################################################/

    // Abrir el archivo
    ifstream ifs("/home/albin/Documents/creado.json");

    // Objeto para leer el JSON
    Json::Reader reader;

    // Objeto para guardar un JSON Object leido
    Json::Value obj;

    // Leer todo el JSON Array o JSON Object que este en el archivo
    reader.parse(ifs, obj);

    // Obtener el tamano del JSON Array
    cout << "Size: " << obj.size() << endl;



    // Obtener el segundo elemento del JSON Array
    Json::Value method = obj[1];

    // Imprimir los valores para ver que lo lea bien
    cout << "Language: " << method["language"].asString() << endl;
    cout << "Id: " << method["id"].asInt() << endl;

}


int main(){
    json_parse();
    return 0;
}

