//------------------------ Хранение данных в процессе работы приложения.
package com.example.myapplication;

import android.app.Application;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

/**
 * Created by Шпуряка on 04.10.2019.
 */

/*
* Способ сделать это - создать собственный подкласс android.app.Application, а затем указать этот
* класс в теге приложения в ваш манифест. Теперь Android автоматически создаст экземпляр этого
* класса и сделает его доступным для всего вашего приложения. Вы можете получить к нему доступ
* из любого context с помощью метода Context.getApplicationContext() (Activity также предоставляет
* метод getApplication(), который имеет тот же эффект).
*
*В манифесте Android вы должны объявить класс, реализующий android.app.Application
* (добавьте атрибут android:name=".MyApplication" в существующий тег приложения):
*
* */

public class ValuesApplication extends Application {

    DatabaseHelper databaseHelper ;  // БД
    SQLiteDatabase db;              // Для работы  с базой данных
    Cursor userCursor;              // Курсор для получения данных из БД в виде курсора

    //==========================Создание переменных доступных из любой части ПО=====================
    //---------------Переменная для примера, потом удалить------
    private String someVariable ;
    //------------------Переменные блока ограничения заданной скорсти-----------
    private Float r_Z_SkMax_M = (float) 0.0;         // Ограничение заданной скорости max
    private Float r_Z_SkMax_P = (float) 0.0;         // Ограничение заданной скорости min
    //------------------Переменные блока задания скорсти------------------------
    private Float ZSkA = (float) 0.0;                // Задание с аналоговог входа
    private Float Z_Skor = (float) 0.0;              // "N#" Задание скорости на выходе
    private Float r_Mashtab_ZSkA = (float) 0.0;      // Задание масштаба для скорости
    //------------------Переменные блока ЗИС------------------------
    private Float r_T_ZISkorP_Razg = (float) 0.0;    // Задание интенстивности разгона
    private Float r_T_ZISkorP_Torm = (float) 0.0;    // Задание интенсивности торможения
    private Float r_T_ZISkorP_Fors = (float) 0.0;    // Задание форсированного режима
    private Float ZISkor = (float) 0.0;              // "N#R" Задание скорости на выходе ЗИС
    //------------------ Переменные Ud and E--------------------------------------------------
    private Float Ud = (float) 0.0;    // Напряжение датчика напряжения
    private Float E = (float) 0.0;     // "E",       &EDS_dop_kod, ЭДС
    //------------------
    private Float OuRegS_dop_kod = (float) 0.0;      // "Id#"
    private Float ZIDN = (float) 0.0;                // "Id#R"
    private Float Id_dop_kod = (float) 0.0;          // "Id"
    private Float Skor = (float) 0.0;                // "N"  - Skor
    private Float S_Alfa = (float) 0.0;              // "L#" - S.Alfa
    private Float S_Alfa_Old = (float) 0.0;          // "L"  - S.Alfa_Old
    private Float V_Fv_zad = (float) 0.0;            // "F#" - V.Fv_zad

    /*,{ "F",       &V.Fv,          }
      ,{ "If#",     &V.Iv_zad,      }
      ,{ "If",      &V.Iv,          }
      ,{ "Lf#",     &V.Alfa,        }
      ,{ "Lf",      &V.Alfa_Old,    }*/



    //=============================Методы get and set===============================================
    //------------------экспериментальная переменная, потом удалить---------------------------------
    public String getSomeVariable() {
        return someVariable;
    }
    public void setSomeVariable(String someVariable) {
        this.someVariable = someVariable;
    }


    //==================Переменные блока задания скорсти ===========================================
    //------- Заданная скорость по аналоговому входу--------------------
    public Float getZSkA() {return ZSkA;}
    public void setZSkA(Float ZSkA) {this.ZSkA = ZSkA;}

    //------- Заданная скорость по уставке--------------------
    public Float getZ_Skor() {return Z_Skor;}
    public void setZ_Skor(Float Z_Skor) {this.Z_Skor = Z_Skor;}

    //------- Заданние масштаба для заданной скорости от аналогового входа---------
    public Float get_r_Mashtab_ZSkA() {return r_Mashtab_ZSkA;}
    public void set_r_Mashtab_ZSkA(Float r_Mashtab_ZSkA) {this.r_Mashtab_ZSkA = r_Mashtab_ZSkA;}
    //==============================================================================================


    //==================Переменные блока ограничения заданной скорсти===============================
    //------- Ограничение заданной скорости max
    public Float getr_Z_SkMax_M() {
        return r_Z_SkMax_M;
    }
    public void setr_Z_SkMax_M(Float r_Z_SkMax_M) {this.r_Z_SkMax_M = r_Z_SkMax_M;}

    //------- Ограничение заданной скорости min
    public Float getr_Z_SkMax_P() {
        return r_Z_SkMax_P;
    }
    public void setr_Z_SkMax_P(Float r_Z_SkMax_P) {this.r_Z_SkMax_P = r_Z_SkMax_P;}
    //==============================================================================================


    //==================Переменные блока ЗИС ===========================================
    //------- Заданная интенсивность разгона--------------------
    public Float getr_T_ZISkorP_Razg() {return r_T_ZISkorP_Razg;}
    public void setr_T_ZISkorP_Razg(Float r_T_ZISkorP_Razg) {this.r_T_ZISkorP_Razg = r_T_ZISkorP_Razg;}

    //------- Заданная интенсивность торможения--------------------
    public Float getr_T_ZISkorP_Torm() {return r_T_ZISkorP_Torm;}
    public void setr_T_ZISkorP_Torm(Float r_T_ZISkorP_Torm) {this.r_T_ZISkorP_Torm = r_T_ZISkorP_Torm;}

    //------- Заданние интенсивность форсирования---------
    public Float getr_T_ZISkorP_Fors() {return r_T_ZISkorP_Fors;}
    public void setr_T_ZISkorP_Fors(Float r_T_ZISkorP_Fors) {this.r_T_ZISkorP_Fors = r_T_ZISkorP_Fors;}

    //------- Заданние интенсивность форсирования---------
    public Float getZISkor() {return ZISkor;}
    public void setZISkor(Float ZISkor) {this.ZISkor = ZISkor;}
    //==============================================================================================

    //------- Напряжение датчика напряжения---------
    public Float get_Ud() {return Ud;}
    public void set_Ud(Float Ud) {this.Ud = Ud;}

    //------- Э.Д.С.---------
    public Float get_E() {return E;}
    public void set_E(Float E) {this.E = E;}
    //==============================================================================================

    //---------------------------------пока не сортирую---------------------------------------------
    public Float get_Idz() {return OuRegS_dop_kod;}
    public void set_Idz(Float OuRegS_dop_kod) {this.OuRegS_dop_kod = OuRegS_dop_kod;}   // "Id#"

    public Float get_IdzR() {return ZIDN;}                                              // "Id#R"
    public void set_IdzR(Float ZIDN) {this.ZIDN = ZIDN;}

    public Float get_Id() {return Id_dop_kod;}                                          // "Id"
    public void set_Id(Float Id_dop_kod ) {this.Id_dop_kod = Id_dop_kod;}

    public Float get_N() {return Skor;}                                                 // "N"
    public void set_N(Float Skor ) {this.Skor = Skor;}

    public Float get_Lz() {return S_Alfa;}                                              // "L#"
    public void set_Lz(Float S_Alfa ) {this.S_Alfa = S_Alfa;}

    public Float get_L() {return S_Alfa_Old;}                                           // "L"
    public void set_L(Float S_Alfa_Old ) {this.S_Alfa_Old = S_Alfa_Old;}


    // Выводим данные в ревизионное окно. Все получилось
    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();

        //sb.append("\n Ud:" + this.Ud);
        //sb.append("\n E:" + this.E);
        sb.append("\n N#R:" + this.getZISkor());
        /*if (this.websites != null) {
            sb.append("\n website: ");
            for (String website : this.websites) {
                sb.append(website + ", ");
            }
        }
        if (this.address != null) {
            sb.append("\n address:" + this.address.toString());
        }*/
        return sb.toString();
    }




// Пока не испльзую, т.к. крашится приложение при попытке открытиия БД  - читаю данные сюда в основном активити
    public void UpdateWithDB( DatabaseHelper databaseHelper,  SQLiteDatabase db, Cursor userCursor){
        //databaseHelper = new DatabaseHelper(getApplicationContext());
        //db = databaseHelper.open(); //открываю БД
        this.databaseHelper = databaseHelper;
        this.db = db;
        this.userCursor = userCursor;
        //произвожу запрос определенного элемента
        this.userCursor =  db.rawQuery("select * from " + DatabaseHelper.TABLE + " where " + DatabaseHelper.COLUMN_name + " = ? ", new String[] {"r.T_ZISkorP_Razg"});
        //userCursor =  db.rawQuery("select * from "+ DatabaseHelper.TABLE, null);
        this.userCursor.moveToFirst();// - необходимо курсором перейтик строке , так как если не перейти, то она будет
        //T_ZISkorP_Razg = userCursor.getString(userCursor.getColumnIndex(DatabaseHelper.COLUMN_values));
        Float var = userCursor.getFloat(userCursor.getColumnIndex(DatabaseHelper.COLUMN_values));
        setr_T_ZISkorP_Razg(var);
        //db.close();
        //userCursor.close();
    }


    public class Params {
        //public static final String ID = "id";
        //public static final String NAME = "name";
        //public static final String COUNTRY = "country";
        //public static final String CITY = "city";
        public static final String NzR = "N#R";
        public static final String NAME = "name";
        public static final String COUNTRY = "country";
        public static final String CITY = "city";

    }
}
