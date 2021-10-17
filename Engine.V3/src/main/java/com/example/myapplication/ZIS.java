package com.example.myapplication;

/*
    * ЗИС расположен в regsc.c
    * // Копия задания, чтобы не портить его в исходнике.
      ZISkor = Z_Skor ;

      ZISkor_Ogr () ; // Проверка Задания Скорости на уставочные ограничения .

 // Задатчик Интенсивности
      if ( Isp._.ZIRS_shunt == 0 ) // Если есть ЗИРC :
      {
        Zad_Intens ( &zi_rs ) ;
       }
    else
    {
        Set_ZI ( &zi_rs , ZISkor ) ; // Инициализация всего ЗИРС .
    }

// входные параметры
     "N#R",     &ZISkor, скорстьнарастания скорост
     "N#",      &Z_Skor,
     "ТемпЗИ-РС'+'  ", &_r.T_ZISkorP_Razg    , 0               , 300              , 10           ,(w*)&_ind_Nom,  1             , 0xff, "мин", _form(1,2,1,2) },
     "ТемпЗИ-РС'-'  ", &_r.T_ZISkorP_Torm    , 0               , 300              , 10           ,(w*)&_ind_Nom,  1             , 0xff, "мин", _form(1,2,1,2) },
     "ТемпЗИ-РС'фрс'", &_r.T_ZISkorP_Fors

    *
    *
    *
    * */
// здесь же мы будем вызывать и проверку на ограничение скорости


import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.support.v4.widget.SimpleCursorAdapter;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;

public class ZIS extends AppCompatActivity {

    public float Z_Skor;
    float ZISkor;
    float r_T_ZISkorP_Razg;
    float r_T_ZISkorP_Torm;
    float r_T_ZISkorP_Fors;
    float r_Z_SkMax_M;
    float r_Z_SkMax_P;

    // Может быть еще необходимо читать конфигурацию, но это потом

    DatabaseHelper databaseHelper;  // БД
    SQLiteDatabase db;              // Для работы  с базой данных
    Cursor userCursor;              // Курсор для получения данных из БД в виде курсора
    SimpleCursorAdapter userAdapter; // В качестве данніх используется БД с помощью Cursor
    TextView ZIS;                 //
    ListView ZIS_List;              //



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_zis);

        databaseHelper = new DatabaseHelper(getApplicationContext());

        ZIS = (TextView) findViewById(R.id.ZIS);
        ZIS_List = (ListView)findViewById(R.id.ZIS_List);
    }

    protected void onResume() {
        super.onResume();
        // ---------------------------Подкл к БД------------------------------------------------------------
        // подключаемся  к скопированной БД
        // открываем подключение
        db = databaseHelper.open();
        //получаем данные из бд в виде курсора
        userCursor =  db.rawQuery("select * from "+ DatabaseHelper.TABLE, null);


        // определяем, какие столбцы из курсора будут выводиться в ListView
        String[] headers = new String[] {DatabaseHelper.COLUMN_name, DatabaseHelper.COLUMN_values};
        // создаем адаптер, передаем в него курсор
        userAdapter = new SimpleCursorAdapter(this, android.R.layout.two_line_list_item,
                userCursor, headers, new int[]{android.R.id.text1, android.R.id.text2}, 0);

        //N_Zad.setText("Н" + String.valueOf(userCursor.getCount()));
        ZIS_List.setAdapter(userAdapter);
    }


    // конструктор
    void ZIS(float Z_Skor, float ZISkor, float r_T_ZISkorP_Razg, float r_T_ZISkorP_Torm, float r_T_ZISkorP_Fors) {
        this.ZISkor = ZISkor;
        this.Z_Skor =  Z_Skor;
        this.r_T_ZISkorP_Fors = r_T_ZISkorP_Fors;
        this.r_T_ZISkorP_Razg = r_T_ZISkorP_Razg;
        this.r_T_ZISkorP_Torm = r_T_ZISkorP_Torm;
    }


    public ArrayList Complete_NzAndNzR(){

        int i = 0;  // счетчик для диалоговых окон
        // Пока не хрена не работает .
        //String messages = new String();
        //int messages = 0;
        // Пробую создать массив
        //int[] messages = new int[20];
        ArrayList messages =  new ArrayList();
        //messages = null;


        //if (ZISkor == 0.0 && Z_Skor > 0.0){

            messages.add(R.string.dialog_nZadR_0_ZIS);  // вобщем необходимо передавать целым номер стп\рокового ресурса
            i++;
        //}

        //if (Z_Skor != ZISkor ){
            messages.add(R.string.dialog_nZad_Zad_ZIS);
           // i++;
        //}
        //Получаем переменные из хранилища
        //Z_Skor// = (float) 0.0;
        // ((ValuesApplication) this.getApplication()).getZ_Skor();
        //CustomDialogFragment dialog = new CustomDialogFragment();
        //dialog.show(getSupportFragmentManager(), "custom");
        //r_Mashtab_ZSkA = ((ValuesApplication) this.getApplication()).get_r_Mashtab_ZSkA();

        ///if (r_Mashtab_ZSkA == 0){
        //Выволим диалог
        //args.putString("phone", "Масштаб задания скорости = 0!");
        //dialog.setArguments(args);
        //dialog.show(getSupportFragmentManager(), "custom");
        //}
        return messages;//messages;
    }

    public void onDestroy(){
        super.onDestroy();
        // Закрываем подключение К БАЗЕ ДАННЫХ.
        db.close();
    }

}
