package com.example.myapplication;

import android.util.JsonWriter;

import java.io.IOException;
import java.io.Writer;

/*Этот файл необходимо переделать для себя что бы записывать распарсенную инфу*/

/**
 * Created by Александр on 13.12.2019.
 */

public class JsonWriterExample {


    public static void writeJsonStream(Writer output/*, Company company*/ ) throws IOException {
        JsonWriter jsonWriter = new JsonWriter(output);
        /*
        *
            Записывает значение JSON (RFC 4627) в поток, по одному токену за раз. Поток включает
            в себя как буквенные значения (строки, числа, логические и нулевые значения), так и начальный и конечный
             разделители объектов и массивов.


            Кодировка JSON
            Чтобы закодировать ваши данные как JSON, создайте новый JsonWriter. Каждый документ JSON должен содержать один
            массив или объект верхнего уровня. Вызывайте методы записи по мере того, как вы просматриваете содержимое структуры,
             при необходимости вкладывая массивы и объекты:
            Чтобы записать массивы, сначала вызовите beginArray (). Запишите каждый из элементов массива с помощью соответствующих
            етодов (булевых) или вложив другие массивы и объекты. Наконец закройте массив, используя endArray ().
            Чтобы написать объекты, сначала вызовите beginObject (). Запишите каждое из свойств объекта, чередуя вызовы name
             (String) со значением свойства. Запишите значения свойств с помощью подходящего (логического) метода value или
              вложив другие объекты или массивы. Наконец закройте объект, используя endObject ().


        * */



        jsonWriter.beginObject();// begin root

        /*jsonWriter.name("id").value(company.getId());
        jsonWriter.name("name").value(company.getName());
        String[] websites= company.getWebsites();

        // "websites": [ ....]
        jsonWriter.name("websites").beginArray(); // begin websites
        for(String website: websites) {
            jsonWriter.value(website);
        }
        jsonWriter.endArray();// end websites

        // "address": { ... }
        jsonWriter.name("address").beginObject(); // begin address
        jsonWriter.name("street").value(company.getAddress().getStreet());
        jsonWriter.name("city").value(company.getAddress().getCity());
        */
        //jsonWriter.endObject();// end address

        // end root
        jsonWriter.endObject();
    }

/*
    public static Company createCompany() {

        Company company = new Company();
        company.setId(123);
        company.setName("Apple");

        String[] websites = { "http://apple.com", "https://jobs.apple.com" };
        company.setWebsites(websites);

        Address address = new Address();
        address.setCity("Cupertino");
        address.setStreet("1 Infinite Loop");

        company.setAddress(address);

        return company;
    }
*/
}