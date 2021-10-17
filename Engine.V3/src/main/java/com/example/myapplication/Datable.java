package com.example.myapplication;

/**
 * Created by Шпуряка on 09.10.2019.
 */

// Этоот интерфес для диалога, еслинеобходимо обратится к активити и удалить например элемент в списке пока оставляю.
    // как его использрвать смотри DialogFragment_Activity
public interface Datable {
    //Единственный метод интерфейса remove получает удаляемый объект в виде параметра name.
    void remove(String name);
}

