package com.example.myapplication;

import android.app.Application;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.widget.Spinner;

import java.util.List;

/**
 * Created by Александр on 24.06.2020.
 */
// Пробую перенести логику сети в отдельный класс
public class Link extends Application {

    String wifis[];                            // Строковый массив для списка сетей
    //Spinner lv;                               // Объект типа список
    int i_w;                                   // вспомогательная переменная
    private MainActivity.WiFiMonitor mWiFiMonitor;           //Объект WiFiMonitor, поиск сети, вывод доступных точек
    String networkSSID = "";//"ESP_D1B9C9";    // Логин - для соединения к сети
    String networkPass = "1234qwer";           // Пароль - для соединения к сети



}
