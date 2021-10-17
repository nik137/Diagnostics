package com.example.myapplication;

import android.app.Activity;
import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;

/*Огляд org.json
* Classes
* ------------------------------------------------------------------------------------------------------------------------------
JSONArray	A dense indexed sequence of values.
        Плотная индексированная последовательность значений.
        Экземпляры этого класса не являются потокобезопасными. Хотя этот класс не является окончательным,
         он не предназначен для наследования и не должен быть разделен на подклассы. В частности, самоопределение
         переопределенными методами не указано. См. Действующий пункт 17 Java «Дизайн и документ или наследование
          или иное запрещение» для получения дополнительной информации.
------------------------------------------------------------------------------------------------------------------------------
JSONObject	A modifiable set of name/value mappings.
        Модифицируемый набор отображений имя / значение.
        JSONObject
        открытый класс JSONObject
        расширяет объект
            java.lang.Object
             ↳ org.json.JSONObject

            Модифицируемый набор отображений имя / значение. Имена являются уникальными ненулевыми строками.
             Значения могут быть любым сочетанием JSONObject, JSONArray, Strings, Booleans, Integer, Longs, Doubles или NULL.
             Значения не могут быть нулевыми, Double # isNaN (), Double # isInfinite () или любого другого типа, не перечисленного здесь.

            Этот класс может приводить значения к другому типу по запросу.

            Когда запрашиваемый тип является логическим, строки будут принудительно выполняться с использованием сравнения без учета
            регистра для значений «истина» и «ложь».

            Когда запрошенный тип является double, другие типы Number будут принудительно вызваны с помощью Number # doubleValue ().
             Строки, которые могут быть вызваны с использованием Double # valueOf (String), будут.

            Когда запрашиваемый тип является целым числом, другие типы чисел будут приведены с помощью Number # intValue ().

            Строки, которые можно привести с помощью Double # valueOf (String), будут преобразованы в int.

            Когда запрошенный тип является длинным, другие типы чисел будут принудительно вызваны с помощью Number # longValue ().

            Строки, которые можно привести с помощью Double # valueOf (String), будут преобразованы в long. Это двухступенчатое

            преобразование с потерями для очень больших значений. Например, строка «9223372036854775806» возвращает длинный
            9223372036854775807.

            Когда запрошенный тип является String, другие ненулевые значения будут принудительно вызваны с использованием
            String # valueOf (Object). Несмотря на то, что значение NULL не может быть принудительно приведено, значение часового
             объекта JSONObject # NULL приведено к строке «NULL».

            Этот класс может искать как обязательные, так и необязательные значения:

            Используйте getType (), чтобы получить обязательное значение. Это происходит с JSONException, если запрошенное имя не
            имеет значения или если значение не может быть приведено к запрошенному типу.

            Используйте optType (), чтобы получить необязательное значение. Это возвращает системное или предоставленное пользователем
            значение по умолчанию, если запрошенное имя не имеет значения или если значение не может быть приведено к запрошенному типу.

            Предупреждение: этот класс представляет значение NULL двумя несовместимыми способами: стандартная ссылка на Java NULL
             и значение Sentinel JSONObject # NULL. В частности, вызов put (name, null) удаляет именованную запись из объекта,
             но put (name, JSONObject.NULL) сохраняет запись, значение которой равно JSONObject.NULL.

            Экземпляры этого класса не являются потокобезопасными. Хотя этот класс не является окончательным, он не предназначен
             для наследования и не должен быть разделен на подклассы. В частности, самоопределение переопределяемыми методами не указано.
              См. Действующий пункт 17 Java «Дизайн и документ или наследование или иное запрещение» для получения
              дополнительной информации.

--------------------------------------------------------------------------------------------------------------------------------------
JSONStringer	Implements JSONObject#toString and JSONArray#toString.
        Реализует JSONObject # toString и JSONArray # toString.
           Реализует JSONObject # toString и JSONArray # toString. Большинство разработчиков приложений должны использовать эти методы напрямую и игнорировать этот API. Например:

         JSONObject object = ...
            String json = object.toString ();

            Стрингеры только кодируют правильно сформированные строки JSON. В частности:

            Стрингер должен иметь ровно один массив или объект верхнего уровня.
            Лексические области должны быть сбалансированы: каждый вызов функции array () должен иметь соответствующий вызов endArray ()
            , а каждый вызов функции () должен иметь соответствующий вызов endObject ().
            Массивы не могут содержать ключи (имена свойств).
            Объекты должны чередовать ключи (имена свойств) и значения.
            Значения вставляются либо с помощью литеральных вызовов значений, либо с помощью вложенных массивов или объектов.
            Вызовы, которые привели бы к искаженной строке JSON, будут с ошибкой JSONException.
            Этот класс не предоставляет возможности для красивой печати (т. Е. С отступом). Для кодирования выходных данных
            с отступом используйте JSONObject # toString (int) или JSONArray # toString (int).

            Некоторые реализации API поддерживают максимум 20 уровней вложенности. Попытки создать более 20 уровней вложенности могут
            потерпеть неудачу с JSONException.

            Каждый стрингер может использоваться для кодирования одного значения верхнего уровня. Экземпляры этого класса не
             являются потокобезопасными. Хотя этот класс не является окончательным, он не предназначен для наследования и не должен
              быть разделен на подклассы. В частности, самоопределение переопределяемыми методами не указано. См. Действующий пункт
              17 Java «Дизайн и документ или наследование или иное запрещение» для получения дополнительной информации.
-----------------------------------------------------------------------------------------------------------------------------------------
JSONTokener	Parses a JSON (RFC 4627) encoded string into the corresponding object.
        Анализирует строку в кодировке JSON (RFC 4627) в соответствующий объект.


        JSONTokener
        открытый класс JSONTokener
        расширяет объект

        java.lang.Object
        ↳ org.json.JSONTokener

        Анализирует строку в кодировке JSON (RFC 4627) в соответствующий объект. Большинству клиентов этого класса нужно будет
        использовать только конструктор и метод nextValue (). Пример использования:

      String json = "{"
         + "  \"query\": \"Pizza\", "
         + "  \"locations\": [ 94043, 90210 ] "
         + "}";

       JSONObject object = (JSONObject) new JSONTokener(json).nextValue();
       String query = object.getString("query");
       JSONArray locations = object.getJSONArray("locations");

        Для лучшей совместимости и производительности используйте JSON, соответствующий RFC 4627, например, сгенерированный
         JSONStringer. По устаревшим причинам этот синтаксический анализ является снисходительным, поэтому успешный анализ
         не означает, что входная строка была допустимой JSON. Все следующие синтаксические ошибки будут игнорироваться:

        Комментарии в конце строки, начинающиеся с // или # и заканчивающиеся символом новой строки.
        Комментарии в стиле C, начинающиеся с / * и заканчивающиеся * /. Такие комментарии не могут быть вложенными.
        Строки без кавычек или «одинарные кавычки».
        Шестнадцатеричные целые числа с префиксом 0x или 0X.
        Восьмеричные целые с префиксом 0.
        Элементы массива, разделенные;.
        Ненужные разделители массивов. Они интерпретируются как если бы null был пропущенным значением.
        Пары ключ-значение, разделенные = или =>.
        Пары ключ-значение, разделенные;.
        Каждый токен может использоваться для анализа одной строки JSON. Экземпляры этого класса не являются потокобезопасными.
        Хотя этот класс не является окончательным, он не предназначен для наследования и не должен быть разделен на подклассы.
         В частности, самоопределение переопределяемыми методами не указано. См. Действующий пункт 17 Java «Дизайн и документ
          или наследование или иное запрещение» для получения дополнительной информации.
          ---------------------------------------------------------------------------------------------------------------
*
*
*
*
*
* */



/**
 * Created by Шпуряка on 17.12.2019.
 */

/*In above source code, isSuccess(String response)
method is used to check whether a status of response is true or false (see in JSON data above).
---
getErrorCode(String response) method is used to get the message of JSON data (see in JSON data above).
---
getInfo(String response) method will parse JSON data. I will describe this method later.

*/
 /*
 * {"status":"true","message":"Data fetched successfully!","data":[{"id":"1","name":"Roger Federer","country":"Switzerland",
 * "city":"Basel","imgURL":"https:\/\/demonuts.com\/Demonuts\/SampleImages\/roger.jpg"},{"id":"2","name":"Rafael Nadal",
 * "country":"Spain","city":"Madrid","imgURL":"https:\/\/demonuts.com\/Demonuts\/SampleImages\/nadal.jpg"},
 * {"id":"3","name":"Novak Djokovic","country":"Serbia","city":"Monaco","imgURL":"https:\/\/demonuts.com\/Demonuts\
 * /SampleImages\/djoko.jpg"},{"id":"4","name":"Andy Murray","country":"United Kingdom","city":"London",
 * "imgURL":"https:\/\/demonuts.com\/Demonuts\/SampleImages\/murray.jpg"},{"id":"5","name":"Maria Sharapova",
 * "country":"Russia","city":"Moscow","imgURL":"https:\/\/demonuts.com\/Demonuts\/SampleImages\/shara.jpg"},
 * {"id":"6","name":"Caroline Wozniacki","country":"Denmark","city":"Odense","imgURL":"https:\
 * /\/demonuts.com\/Demonuts\/SampleImages\/woz.jpg"},{"id":"7","name":"Eugenie Bouchard","country":"Canada","city":
 * " Montreal","imgURL":"https:\/\/demonuts.com\/Demonuts\/SampleImages\/bou.png"},{"id":"8","name":"Ana Ivanovic",
 * "country":"Serbia","city":"Belgrade","imgURL":"https:\/\/demonuts.com\/Demonuts\/SampleImages\/iva.jpg"}]}
  */



public class ParseContent {

    private final String KEY_SUCCESS = "status";
    private final String KEY_MSG = "message";
    private Activity activity;  // для доступа к моему контексту активити из не активити

    ArrayList<HashMap<String, String>> arraylist;

    public ParseContent(Activity activity) {
        this.activity = activity;
    }// для доступа к моему контексту активити из не активити



    public boolean isSuccess(String response)   {

        try {
            JSONObject jsonObject = new JSONObject(response);
            if (jsonObject.optString(KEY_SUCCESS).equals("true")) {
                return true;
            } else {

                return false;
            }

        } catch (JSONException e) {
            e.printStackTrace();
        }
        return false;
    }

    public String getErrorCode(String response) {

        try {
            JSONObject jsonObject = new JSONObject(response);
            return jsonObject.getString(KEY_MSG);

        } catch (JSONException e) {
            e.printStackTrace();
        }
        Log.d("I_am","Hello");
        return "No data";
    }

    /*Our JSON data starting from ‘{‘ bracket so it is JSONObject. So the starting ‘{‘ is parse by below line*/
    //public ArrayList<PlayersModel> getInfo(String response) {
    //У меня этот класс так и не заработал - вылетает - оставляю на потом
    public ArrayList<ValuesApplication> getInfo(String response) {
            //ArrayList<PlayersModel> playersModelArrayList = new ArrayList<>();
            ArrayList<ValuesApplication> playersModelArrayList = new ArrayList<>();
        //playersModelArrayList = null;
        try {
            /*Our JSON data starting from ‘{‘ bracket so it is JSONObject. So the starting ‘{‘ is parse by below line*/
           JSONObject jsonObject = new JSONObject(response);
            //if (jsonObject.getString(KEY_SUCCESS).equals("true")) {  меня нету проверки статуса

                //arraylist = new ArrayList<HashMap<String, String>>();
                /*After that, we have key named “data” and its value is starting from
                ‘[‘ bracket which is JSONArray. It is parse by following line.
                  JSONArray dataArray = jsonObject.getJSONArray("data");*/


                //Здесь создается массив, после ключа. Наверное мне пока не надо
                // у МЕНЯ НЕТУ КЛЮЧА/ Это емли двойной и более массив
                //JSONArray dataArray = jsonObject.getJSONArray("");

                /*
                * Now data array contains 8 objects so they are parsed by for loop and it is stored in instance of PlayersModel Class.
                    for (int i = 0; i < dataArray.length(); i++) {
                    PlayersModel playersModel = new PlayersModel();
                    JSONObject dataobj = dataArray.getJSONObject(i);
                    playersModel.setName(dataobj.getString(AndyConstants.Params.NAME));
                    playersModel.setCountry(dataobj.getString(AndyConstants.Params.COUNTRY));
                    playersModel.setCity(dataobj.getString(AndyConstants.Params.CITY));
                    playersModelArrayList.add(playersModel);
                }
                After getting playersModelArrayList from getInfo() method, listview is populated using custom adapter.
                So thats all for JSON Parsing Android Studio example, Feel free to comment your queries and reviews. Thank you
                * */

                /*for (int i = 0; i < dataArray.length(); i++) {
                    PlayersModel playersModel = new PlayersModel();
                    JSONObject dataobj = dataArray.getJSONObject(i);
                    playersModel.setName(dataobj.getString(AndyConstants.Params.NAME));
                    playersModel.setCountry(dataobj.getString(AndyConstants.Params.COUNTRY));
                    playersModel.setCity(dataobj.getString(AndyConstants.Params.CITY));
                    playersModelArrayList.add(playersModel);
                }*/
                //double Ud = jsonObject.getDouble("N#R");
                //не работает вылетает
                //ValuesApplication storage = new ValuesApplication(); // Создаем объект своего хранилища
                //JSONObject dataObj = dataArray.getJSONObject(0);
                //storage.setZISkor((float) dataObj.getDouble(ValuesApplication.Params.NzR));
                //storage.setZISkor((float)Ud);

           // playersModelArrayList.add(storage);



            //}

        } catch (JSONException e) {
            e.printStackTrace();
        }
        return playersModelArrayList;
    }
    // Здесь извлекаем данные из формата
    public ValuesApplication readDataJSONWeb(String response) { //мое извращение
        // временнные переменные чтобы принять данные и положить в локальное хранилище
        // в будущем оптимизировать, просто без низ сразу ложжить в хранилище и читать с формата
        double Idz = 0    ;     // 1 -
        double NzR = 0    ;     // 2 -
        double Ud = 0     ;     // 3 -
        double Nz = 0     ;     // 4 -
        double IdzR = 0   ;     // 5 -
        double Id = 0     ;     // 6 -
        double Lz = 0     ;     // 7 -
        double L = 0      ;     // 8 -
        double N = 0      ;     // 9 -
        double Fz = 0     ;     // 10 - V.Fv_zad
        double F = 0      ;     // 11 - V.Fv
        double Ifz = 0    ;     // 12 - V.Iv_zad
        double If = 0     ;     // 13 - V.Iv
        double Lfz = 0    ;     // 14 - V.Alfa
        double Lf = 0     ;     // 15 - V.Alfa_Old
        double E = 0      ;     // 16 - "E" - EDS_dop_kod

        // Read content of company.json            ( data.json )
        // String jsonText = readText(context, R.raw.data);    // читаем содержимое файла
        ValuesApplication storage_json = new ValuesApplication();
        try {

            JSONObject jsonRoot = new JSONObject(response);     // содаем объект
            //Извлекаем данные из файла. Почему нету Float???????????????????????????????????
            // Принимаем формат JSON с контроллера, где проверяем наличие названий переменных, если есть
            // считываем если нету просто выходим
            if (jsonRoot.has("N#R")){                // 1
                NzR = jsonRoot.getDouble("N#R");
            }
            if (jsonRoot.has("Ud")){                 // 2
                Ud = jsonRoot.getDouble("Ud");
            }
            if (jsonRoot.has("N#")){
                Nz = jsonRoot.getDouble("N#");
            }
            if (jsonRoot.has("Id#")){
                Idz = jsonRoot.getDouble("Id#");
            }
            if (jsonRoot.has("Id#R")){
                IdzR = jsonRoot.getDouble("Id#R");
            }
            if (jsonRoot.has("Id")){
                Id = jsonRoot.getDouble("Id");
            }
            if (jsonRoot.has("L#")){
                Lz = jsonRoot.getDouble("L#");
            }
            if (jsonRoot.has("L")){
                L = jsonRoot.getDouble("L");
            }
            if (jsonRoot.has("N")){
                N = jsonRoot.getDouble("N");
            }
            if (jsonRoot.has("Fz")){
                Fz = jsonRoot.getDouble("Fz");
            }
            if (jsonRoot.has("F")){
                F = jsonRoot.getDouble("F");
            }
            if (jsonRoot.has("If#")){
                Ifz = jsonRoot.getDouble("If#");
            }
            if (jsonRoot.has("If")){
                If = jsonRoot.getDouble("If");
            }
            if (jsonRoot.has("Lf#")){
                Lfz = jsonRoot.getDouble("Lf#");
            }
            if (jsonRoot.has("Lf")){
                Lf = jsonRoot.getDouble("Lf");
            }
            if (jsonRoot.has("E")){                // 16 - "E" - EDS_dop_kod
                E = jsonRoot.getDouble("E");
            }

            //double E = jsonRoot.getDouble("E");
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
            //(ValuesApplication) this.getApplication()).getr_T_ZISkorP_Razg();
            //Context.getApplicationContext()
           // storage_json.setZISkor((float) 91.0);
            //((ValuesApplication) this.getApplication()).setZISkor((float)65.0);
            //((Activity)
            ((ValuesApplication)activity.getApplication()).setZISkor((float)NzR);   //"N#R" Закидываем в глобальное хранилище
            storage_json.setZISkor((float) NzR); // это уже другое, объекта  storage_json НЕ ПУТАТЬ
            ((ValuesApplication)activity.getApplication()).setZ_Skor((float)Nz);    //"N#"
            storage_json.setZ_Skor((float) Nz);
            ((ValuesApplication)activity.getApplication()).set_Ud((float)Ud);       //"Ud"
            storage_json.set_Ud((float) Ud);
            ((ValuesApplication)activity.getApplication()).set_Idz((float)Idz);     //"Id#"
            storage_json.set_Idz((float) Idz);
            ((ValuesApplication)activity.getApplication()).set_IdzR((float)IdzR);   //"Id#R"
            storage_json.set_IdzR((float) IdzR);
            ((ValuesApplication)activity.getApplication()).set_Id((float)Id);       //"Id"
            storage_json.set_Id((float) Id);
            ((ValuesApplication)activity.getApplication()).set_N((float)N);         //"N"
            storage_json.set_N((float) N);
            ((ValuesApplication)activity.getApplication()).set_E((float)E);         //"Id"
            storage_json.set_E((float) E);
            ((ValuesApplication)activity.getApplication()).set_Lz((float)Lz);       //"L#"
            storage_json.set_Lz((float) Lz);
            ((ValuesApplication)activity.getApplication()).set_L((float)L);         //"L"
            storage_json.set_L((float) L);



            //storage_json.set_Ud((float) Ud);
            //storage_json.set_E((float)E);
        } catch (JSONException e) {
        e.printStackTrace();
        }
        return storage_json; // это потомок глобального хранилище, не копия, к него свои жданные
    }



}