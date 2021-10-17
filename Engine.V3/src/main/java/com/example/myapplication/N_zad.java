package com.example.myapplication;
/* Сюда добавляем логику (в блок). Сюда передаем параметры с мейна. Здесь в зависимости от логики
  * передаем сообщение которое отправляем в диалог в мейн */
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.support.v4.widget.SimpleCursorAdapter;
import android.support.v7.app.AppCompatActivity;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;

public class N_zad extends AppCompatActivity {

    //Создадим свойства класса это будут наши входные параметры прочитаные уже из базы данных и взятые из локального хранилища
   // По этим свойсвам построим основную логику потом уже можна добавить всякие мелочи

    //---------------для вывода диалога------------------------
    //CustomDialogFragment dialog = new CustomDialogFragment();
    //Bundle args = new Bundle();
    //public N_zad(){
    //    Z_Skor =0 ;
    //    r_Mashtab_ZSkA = 0;
    // }

    //----------------------------------- Итак свойства класса
    public float Z_Skor;               // Заданная скорость локальная
    public float r_Mashtab_ZSkA;       // Масштаб
    public float ZSkA;                 // заданная скорость с аналогового входа Аналоговый вход
                                       // Заданная скорость от уставки

   // Может быть еще необходимо читать конфигурацию, но это потом

    DatabaseHelper databaseHelper;  // БД
    SQLiteDatabase db;              // Для работы  с базой данных
    Cursor userCursor;              // Курсор для получения данных из БД в виде курсора
    SimpleCursorAdapter userAdapter; // В качестве данніх используется БД с помощью Cursor
    TextView N_Zad;                 //
    ListView N_Zad_List;              //

///////==================================Методы=----------------------------------------------------------------
    //1.  -------------------------------------------Выводить параметры - этитм у меня уже занимается активити
    // Которая выводит мои данные блока из БД
//----------------------------------------------------------------------------------------------------------------

    //---------------------------------------------------------------------------------------------------------
    // 2. Метод сравнения данных, если пр двух заданных на выходе нет скорости то мигает блок и выводит диалоговое
    // окно, которое посит внести масштаб
    //-------------------------------------------------------------------------------------------------------------
    // Изменяю метод что он возвраш\щает массив
    public ArrayList Complete_N_zad(float ZSkA, float r_Mashtab_ZSkA, float Z_Skor){

        int i = 0; // счетчик для сообщений
        // Пока не хрена не работает .
        //String messages = new String();
        //int messages = 0;
        // Пробую создать массив
        //int[] messages = new int[20];
        ArrayList messages = new ArrayList();

        //messages = null;
        this.r_Mashtab_ZSkA = r_Mashtab_ZSkA;
        this.Z_Skor =  Z_Skor;
        this.ZSkA = ZSkA;

        if (r_Mashtab_ZSkA == 0.0){
            //messages = "jjjj" + R.string.dialog_nZad_Scale;// так о\работате но возвращает первую строку и далее набор цифр
            //messages = "jjjj" + getString(R.string.dialog_nZad_Scale);//так крашится.
            //messages = "jjjj" + getResources().getString(R.string.dialog_nZad_Scale); //краш
            //messages = "jjjj" + context.getString(R.string.dialog_nZad_Scale); // краш
            //messages = "jjjj" + Resources.getSystem().getString(R.string.dialog_nZad_Scale);// краш
            messages.add(R.string.dialog_nZad_Scale);  // вобщем необходимо передавать целым номер стп\рокового ресурса
            i++;
        }

        if (ZSkA > 0 ){
            messages.add(R.string.dialog_nZad_Zad);
            i++;
        }
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


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_n_zad);

        databaseHelper = new DatabaseHelper(getApplicationContext());

        N_Zad = (TextView) findViewById(R.id.N_Zad);
        N_Zad_List = (ListView)findViewById(R.id.N_Zad_List);
    }

    @Override
    protected void onResume() {
        super.onResume();
        // ---------------------------Подкл к БД------------------------------------------------------------
        // подключаемся  к скопированной БД
        // открываем подключение
        db = databaseHelper.open();
        //получаем данные из бд в виде курсора
        //userCursor =  db.rawQuery("select * from "+ DatabaseHelper.TABLE, null);
        //userCursor =  db.rawQuery("select * from " + DatabaseHelper.TABLE + " where " + DatabaseHelper.COLUMN_name + " like ? and like ?", new String[]{"%" + constraint1.toString() + "%", "%" + constraint2.toString() + "%"});
        //rawQuery("SELECT id, name FROM people WHERE name = ? AND id = ?", new String[] {"David", "2"});
        //userCursor =  db.rawQuery("select * from " + DatabaseHelper.TABLE + " where " + DatabaseHelper.COLUMN_name + " = ? ", new String[] {"ZSkA" });   - выводит одно поле
        userCursor =  db.rawQuery("select * from " + DatabaseHelper.TABLE + " where " + DatabaseHelper.COLUMN_name + " = ? or "+ DatabaseHelper.COLUMN_name + "  = ? or " + DatabaseHelper.COLUMN_name + " = ? ", new String[] {"ZSkA", "Z_Skor", "r.Mashtab_ZSkA" });

        //"select * from " + DatabaseHelper.TABLE + " where " + DatabaseHelper.COLUMN_NAME + " like ? and like ?", new String[]{"%" + constraint1.toString() + "%", "%" + constraint2.toString() + "%"}
        //вам надо выражение типа
        //select * from table where column1 like constraint1 and column2 like constraint2

        // определяем, какие столбцы из курсора будут выводиться в ListView
        String[] headers = new String[] {DatabaseHelper.COLUMN_name, DatabaseHelper.COLUMN_values};
        // создаем адаптер, передаем в него курсор
        userAdapter = new SimpleCursorAdapter(this, android.R.layout.two_line_list_item,
                userCursor, headers, new int[]{android.R.id.text1, android.R.id.text2}, 0);

        //N_Zad.setText("Н" + String.valueOf(userCursor.getCount()));
        N_Zad_List.setAdapter(userAdapter);
        //Complete_N_zad();

    }

    /*Логика слдеующая если нет - опрашиваем все каналы, которыыми можно подать задание.
        * Итак "N#    &Z_Skor
        * 1. этап читаем конфигурацию -  оставм это на потом
        * 2 - считать возможные каналы задания если они задаются, а Z_Skor = 0, то
        *
         * 2.1 - проверка цепочки переменных и уставок идущих до жанной переменной, а это
          * "N#-масштаб - &_r.Mashtab_ZSkA
          *2.2 - проверка перемнных идущих с ацп, атакже заданных уставок, если задание от уставки
          *
          *3. при считывании канала или уставки, если есть какието значения а на входе 0 блок должен мигать
          *   красным, если есть какоеть значение зеленным, должен выодить значение
          *   если подпирает границы до ограничители также дожны мигать красным.
          * и выводить сообщения
          *
          *
          *
          * */
    // Разметка в этом уроке на подобие 0491
    public void InputZad (){
        // проверка заданий проверяяем ацп


    }
    public void onDestroy(){
        super.onDestroy();
        // Закрываем подключение К БАЗЕ ДАННЫХ.
        db.close();
    }



}



/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////
-------------------------------------------------------------------------------------------------------------
* Создание таблицы  +  программное создание таблицы (https://devpad.ru/post/item/6/android-overview-tablelayout.html)
*------------------------------------ Android. Обзор TableLayout------------------------------------
Диспетчер компоновки TableLayout является расширением компоновки LinearLayout.
 Этот диспетчер выстраивает вложенные элементы в строки и столбцы, как и в любой таблице.
Для использования этой компоновки, нужно в контейнере TableLayout создать элементы TableRow,
которые будут являться строками и выстраиваться вертикально. Внутри TableRow можно размещать другие элементы
и они будут выстраиваться горизонтально.
*
*
* Вначале создаём контейнер разметки TableLayout и вкладываем в него 3 элемента TableRow.
*  У нас получилось 4 строки в таблице. Далее создаём столбцы таблицы и для этого в каждый элемент
*  TableRow вкладываем по 3 элемента TextView. В первом напишем названия столбцов, а во вторых названия.
*  Для того, чтобы визуально отделить заголовки таблицы, мы зададим фоновый цвет для первого TableRow.
<TableRow android:background="#cccccc"></TableRow>
Чтобы содержимое элементов TableRow растянулось на всю ширину макета, нужно указать у TableLayout значение
атрибуте android:stretchColumns равным звёздочке.
android:stretchColumns="*"
Вообще, через запятую можно указать порядковые номера столбцов, которые нужно растянуть. Номера столбцов
указываем начиная с нуля. Остальные займут минимум места.
Давайте рассмотрим подробнее эту функциональность на примере.

Например В этом примере, в первой таблице мы задали атрибут strechColumn равным "0,3,5"

 столбцы с индексами 0, 3 и 5 растянулись равномерно, а остальные заняли минимум места. Как я уже писал выше, и
 ндекс считается с нуля.
Для сравнения, у следующей таблицы мы указали, чтобы растянулись все столбцы, значением звёздочкой
android:stretchColumns="*".

На JAVA это задаётся методом setStretchAllColumns(true)(для растягивания всех столбцов).
Чтобы задать определённые столбцы, нужно воспользоваться методом setColumnStretchable(int index, boolean isStretchable).
Для каждого индекса нужно вызывать этот метод отдельно, например:
// эквивалент XML атрибута android:stretchColumns="0,3,5"
tableLayout.setColumnStretchable(0, true);
tableLayout.setColumnStretchable(3, true);
tableLayout.setColumnStretchable(5, true);


Атрибут layout_span
Если Вы знакомы с построением таблиц, на HTML, то наверняка знаете атрибут colspan у тега ,
 который отвечает за объединение нескольких столбцов в один. Так вот, мы тоже можем это сделать в нашей
 компоновке при помощи атрибута android:layout_span у вложенного элемента в TableRow и указав значение в
 виде целого числа, означающего количество объединённых столбцов.
 Как видите, первая кнопка растянулась на 3 столбца, а третья на 2.

Программно это можно реализовать при помощи вложенного в TableLayout класса LayoutParams.
Пример:
Button button1 = (Button) findViewById(R.id.btn1);
TableRow.LayoutParams params = (TableRow.LayoutParams) button1.getLayoutParams();
params.span = 3;
button1.setLayoutParams(params);

Button button3 = (Button) findViewById(R.id.btn3);
TableRow.LayoutParams params3 = (TableRow.LayoutParams) button3.getLayoutParams();
params3.span = 2;
button3.setLayoutParams(params3);


Рамка в таблице
К сожалению, наш контейнер не может создать рамку для таблицы. Разработчики Android почему то не удосужились
добавить эту функциональность. Ну что же,  попробуем её создать сами.
Схема такая – задаём у корневого элемента TableLayout атрибут background в виде цвета, например
<TableLayout xmlns:android="http://schemas.android.com/apk/res/android"
             android:layout_height="wrap_content"
             android:layout_width="fill_parent"
             android:stretchColumns="*"
             android:background="#000">
Мы установили фоновый цвет в чёрный и это будет цвет нашей рамки.
Далее просто задаём у элементов, вложенных в TableRow другой фоновый цвет. Он будет перекрывать фоновый цвет элемента
TableLayout и тем самым у нас получается подобие рамки между элементами.
Рассмотрим полный код:


Программное создание таблицы
Пример 4
TableLayout tableLayout = new TableLayout(this);
tableLayout.setLayoutParams(new TableLayout.LayoutParams(
        ViewGroup.LayoutParams.FILL_PARENT,
        ViewGroup.LayoutParams.WRAP_CONTENT
        ));
tableLayout.setStretchAllColumns(true);

TextView textView1 = new TextView(this);
textView1.setText("Column 1");
TextView textView2 = new TextView(this);
textView2.setText("Column 2");
TextView textView3 = new TextView(this);
textView3.setText("Column 3");

TextView textView4 = new TextView(this);
textView4.setText("Column 4");
TextView textView5 = new TextView(this);
textView5.setText("Column 5");
TextView textView6 = new TextView(this);
textView6.setText("Column 6");

TextView textView7 = new TextView(this);
textView7.setText("Column 7");
TextView textView8 = new TextView(this);
textView8.setText("Column 8");
TextView textView9 = new TextView(this);
textView9.setText("Column 9");

TableRow tableRow1 = new TableRow(this);
TableRow tableRow2 = new TableRow(this);
TableRow tableRow3 = new TableRow(this);

tableRow1.addView(textView1);
tableRow1.addView(textView2);
tableRow1.addView(textView3);

tableRow2.setBackgroundColor(0xffcccccc);
tableRow2.addView(textView4);
tableRow2.addView(textView5);
tableRow2.addView(textView6);

tableRow3.addView(textView7);
tableRow3.addView(textView8);
tableRow3.addView(textView9);

tableLayout.addView(tableRow1);
tableLayout.addView(tableRow2);
tableLayout.addView(tableRow3);
setContentView(tableLayout);
-------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

*
*
* */