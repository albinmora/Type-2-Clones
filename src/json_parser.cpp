#include "../include/json_parser.hpp"
#include <vector>

using namespace std;

vector<int> get_num_op(Json::Value body);

void get_num_op_aux(Json::Value sub_body, vector<int> &res){
	/*
	 * res[0] = numero lineas
	 * res[1] = numero if
	 * res[2] = numero bucles
	 * res[3] = numero de valores de retorno
	 * res[4] = numero de variables locaes
	 * res[5] = numero de llamadas a funciones.
	 * 
	 */
	vector<int> temp(6);
	
	for (auto const& id : sub_body.getMemberNames()) {
		if(id == "condition" && sub_body.isMember("initBody") && sub_body.isMember("iterationBody")){
			res[2] ++;
			res[0] ++;
		}else if(id == "condition"){
			res[1] ++;
			res[0] ++;
		}else if(id == "thenBody" || id == "elseBody" || id == "body"){
			if(sub_body[id].isMember("subStatements")) 
				temp = get_num_op(sub_body[id]["subStatements"]);
			else  
				temp = get_num_op(sub_body[id]);
			
			res[0] += temp[0];
			res[1] += temp[1];
			res[2] += temp[2];
			res[3] += temp[3];
			res[4] += temp[4];
			res[5] += temp[5];
		
		}else if(id == "returnValue"){ 
			res[0] ++;
			res[3] ++;
		}else if(id == "declOrDefn"){
			if(sub_body[id].isMember("fragments"))
				res[4] =  sub_body[id]["fragments"].size() ;
		}
		else if(id == "expression"){
			if(sub_body[id].isMember("functionName"))
				res[5] ++;
			res[0] ++;
		}
	}
}
vector<int> get_num_op(Json::Value body){
	/*
	 * res[0] = numero lineas
	 * res[1] = numero if
	 * res[2] = numero bucles
	 * res[3] = numero de valores de retorno
	 * 
	 */
	vector<int> res = {0, 0, 0, 0, 0, 0};

	if(body.type() == Json::arrayValue)
		for(auto const& sub_body : body) get_num_op_aux(sub_body, res);
	else get_num_op_aux(body, res);

	return res;
}

int get_num_param(Json::Value body){
	Json::Value params;
	if(body.isMember("formalParameters")){
		params = body["formalParameters"];
		return params.size();
	} cout << "tiene" << endl;
	return 0;
}

int json_parse(){

	vector<int> num_if;
	
	ofstream ofs;
	
	// Objeto para escribir el JSON en el archivo
	Json::StyledWriter sw;
	
	// JSON Object
	Json::Value writeValue;

	// JSON Array
	Json::Value array;

	// Abrir el archivo
	ifstream ifs("result.json");
	
	// Se abre o crea el archivo
	ofs.open("/home/albin/Documents/creado1.json");

	// Objeto para leer el JSON
	Json::Reader reader;

	// Objeto para guardar un JSON Object leido
	Json::Value obj_method;
	Json::Value class_name;
	Json::Value obj_tmp_method;
	Json::Value obj_tmp_rer;

	Json::Value body;

	// Leer todo el JSON Array o JSON Object que este en el archivo
	reader.parse(ifs, obj_method);

	for(auto const& files: obj_method){
		// Imprimir los valores para ver que lo lea bien
		obj_tmp_method = files["opensScope"]["declOrDefn"][0]["aggregateType"]
															  ["opensScope"]["declOrDefn"];
		class_name = files["opensScope"]["declOrDefn"][0]["aggregateType"]["nameString"];
		for(auto const& method : obj_tmp_method){
			cout << "Method Name: " << method["identifierName"]["nameString"] << endl;
			cout << "Class Name: " <<  class_name["nameString"] << endl;
			//cout << "Id: " << obj[0].asInt() << endl;
			if(method.isMember("body")) {
				obj_tmp_rer = method["body"]["subStatements"];
				num_if = get_num_op(obj_tmp_rer);
				
				cout << "Num param " << get_num_param(method) <<endl;
				cout << "Num lines " << num_if[0] <<endl;
				cout << "Num if " << num_if[1] <<endl;
				cout << "Num for " << num_if[2] <<endl;
				cout << "Num returns " << num_if[3] <<endl;
				cout << "Num local " << num_if[4] <<endl;
				
				writeValue["LN"]  = num_if[0];
				writeValue["IF"]  = num_if[1];
				writeValue["CIC"] = num_if[2];
				writeValue["RTN"] = num_if[3];
				writeValue["ARG"] = get_num_param(method);
				writeValue["LOC"] = num_if[4];
				writeValue["FUN"] = num_if[5];
				writeValue["class"] = class_name["nameString"];
				writeValue["name"] = method["identifierName"]["nameString"];
				
				array.append(writeValue);
			}
		}
	}
	// Escribo el JSON Array al archivo
	ofs << sw.write(array);

	// Se cierra el archivo
	ofs.close();

	return 0;
}



