/* Заставка при загрузке приложюения по времени и без. Класс перенаправляет в основной слой */
package com.example.diagnostics;

import android.content.Intent;
import android.graphics.drawable.Animatable;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.widget.ImageView;

/**
 * Created by Шпуряка on 24.09.2019.
 */

public class SplashActivity extends AppCompatActivity {
    /** Duration of wait **/
    private final int SPLASH_DISPLAY_LENGTH = 5000;
    ImageView androidView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash);  // Для отключения активити - закоментируйте.
        androidView =(ImageView)findViewById(R.id.android);
        showDialog();
        //--------------------Вариант без выдержки времени------------------------
        /*  Заставка без выдержки времени, время тратится только на загрузку. Если загрузка моментальная,
         то мы ничего и не увидим.
          Intent intent = new Intent(this, MainActivity.class);
          startActivity(intent);
          finish(); */
        //--------------------Выриант с выдержкой времени-------------------------
        /* New Handler to start the Menu-Activity and close this Splash-Screen after some seconds.*/
        new Handler().postDelayed(new Runnable(){
            @Override
            public void run() {
                /* Create an Intent that will start the Menu-Activity. */
                Intent mainIntent = new Intent(SplashActivity.this,MainActivity.class);
                SplashActivity.this.startActivity(mainIntent);
                SplashActivity.this.finish();
            }
        }, SPLASH_DISPLAY_LENGTH);
    }
    public void showDialog() {
        ((Animatable) androidView.getDrawable()).start();
    }
}