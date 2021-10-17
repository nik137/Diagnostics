//пример взят из  https://o7planning.org/ru/10459/android-json-parser-tutorial
/*Руководство Android JSON Parser
 Чтение из raw  - файла data.json
 для чтения используеттся файл в оригинале Company, где прописаны геты и сеты, я же подключаю свое локальное хранилище
 с гтеами и сетами
*
*
*
*
* */

package com.example.myapplication;

import android.content.Context;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

/**
 * Created by Шпуряка on 12.12.2019.
 */
// Читаем из файла
public class ReadJSONExample {

    // Read the company.json (data.json) file and convert it to a java object.
    //public static Company readCompanyJSONFile(Context context) throws IOException,JSONException {   - оригинал
    // Метод непосредствено читаем и выбираем данные(парсим)
    public static ValuesApplication readDataJSONFile(Context context) throws IOException,JSONException { //мое извращение
        // Read content of company.json            ( data.json )
        String jsonText = readText(context, R.raw.data);    // читаем содержимое файла
        JSONObject jsonRoot = new JSONObject(jsonText);     // содаем объект
        //Извлекаем данные из файла. Почему нету Float???????????????????????????????????
        double Ud = jsonRoot.getDouble("Ud");
        double E = jsonRoot.getDouble("E");
        //int id= jsonRoot.getInt("id");
        //String name = jsonRoot.getString("name");
        //JSONArray jsonArray = jsonRoot.getJSONArray("websites");
        //String[] websites = new String[jsonArray.length()];

        // Для извлечения массива
        //for(int i=0;i < jsonArray.length();i++) {
        //    websites[i] = jsonArray.getString(i);
        //}

        //JSONObject jsonAddress = jsonRoot.getJSONObject("address");
        //String street = jsonAddress.getString("street");
        //String city = jsonAddress.getString("city");
        //Address address= new Address(street, city);

        //Теперь упакуем полученные данные в локальное хранилище

        //Company company = new Company();
        //company.setId(id);
        //company.setName(name);
        //company.setAddress(address);
        //company.setWebsites(websites);
        //return company;

        ValuesApplication storage_json = new ValuesApplication();
        storage_json.set_Ud((float) Ud);
        storage_json.set_E((float)E);
        return storage_json;
    }


    // Функция считывания с файла
    private static String readText(Context context, int resId) throws IOException {
        InputStream is = context.getResources().openRawResource(resId);
        BufferedReader br= new BufferedReader(new InputStreamReader(is));
        StringBuilder sb= new StringBuilder();
        String s= null;
        while((  s = br.readLine())!=null) {
            sb.append(s);
            sb.append("\n");
        }
        return sb.toString();
    }
}
