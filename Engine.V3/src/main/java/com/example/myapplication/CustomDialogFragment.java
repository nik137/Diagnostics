/*
* Класс фрагмента содержит всю стандартную функциональность фрагмента с его жизненным циклом,
 * но при этом наследуется от класса DialogFragment, который добавляет ряд дополнительных функций.
 * И для его создания мы можем использвоать два способа:

Переопределение метода onCreateDialog(), который возвращает объект Dialog.
Использование стандартного метода onCreateView().
*
* Для создания диалогового окна в методе onCreateDialog() применяется класс AlertDialog.Builder.
* С помощью своих методов он позволяет настроить отображение диалогового окна:

setTitle: устанавливает заголовок окна

setView: устанавливает разметку интерфейса окна

setIcon: устанавливает иконку окна

setPositiveButton: устанавливает кнопку подтверждения действия

setNeutralButton: устанавливает "нейтральную" кнопку, действие которой может отличаться от действий подтверждения или отмены

setNegativeButton: устанавливает кнопку отмены

setMessage: устанавливает текст диалогового окна, но при использовании setView данный метод необязателен
или может рассматриваться в качестве альтернативы, если нам надо просто вывести сообщение.

create: создает окно

В данном же случае диаговое кно просто выводит некоторое сообщение.
*
*
*
*
*
*
* */

package com.example.myapplication;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;

/**
 * Created by Шпуряка on 09.10.2019.
 */

public class CustomDialogFragment extends DialogFragment {
    boolean mDismissed;
    boolean mShownByMe;

    public static CustomDialogFragment newInstance(int title, String phone,int id) {
        CustomDialogFragment frag = new CustomDialogFragment();
        Bundle args = new Bundle();
        args.putInt("title", title);
        args.putCharSequence("phone", phone);
        args.putInt("case", id);
        frag.setArguments(args);
        return frag;
    }
    public void show(FragmentManager manager, String tag) {
        mDismissed = false;
        mShownByMe = true;
        FragmentTransaction ft = manager.beginTransaction();
        ft.add(this, tag);
        //ft.commit();
        ft.commitAllowingStateLoss();
    }


    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        int title = getArguments().getInt("title");
        String phone = getArguments().getString("phone");
        int id = getArguments().getInt("case");

        AlertDialog builde = null;
        switch (id) {

            case 1:
                builde = new AlertDialog.Builder(getActivity())
                        .setIcon(android.R.drawable.ic_dialog_alert)
                        //.setMessage("Вы хотите удалить " + phone + "?")
                        .setMessage(phone)
                        .setTitle(title)
                        .setPositiveButton(R.string.alert_dialog_ok,
                                new DialogInterface.OnClickListener() {
                                    public void onClick(DialogInterface dialog, int whichButton) {
                                        ((MainActivity) getActivity()).doPositiveClickD();
                                    }
                                }
                        )
                        .setNegativeButton(R.string.alert_dialog_cancel,
                                new DialogInterface.OnClickListener() {
                                    public void onClick(DialogInterface dialog, int whichButton) {
                                        ((MainActivity) getActivity()).doNegativeClickD();
                                    }
                                }
                        )
                        .create();
                break;

            case 2:
                builde = new AlertDialog.Builder(getActivity())
                        .setIcon(android.R.drawable.ic_dialog_alert)
                        //.setMessage("Вы хотите удалить " + phone + "?")
                        .setMessage(phone)
                        .setTitle(title)
                        .setPositiveButton(R.string.alert_dialog_ok,
                                new DialogInterface.OnClickListener() {
                                    public void onClick(DialogInterface dialog, int whichButton) {
                                        ((MainActivity) getActivity()).doPositiveClickN();
                                    }
                                }
                        )
                        .setNegativeButton(R.string.alert_dialog_cancel,
                                new DialogInterface.OnClickListener() {
                                    public void onClick(DialogInterface dialog, int whichButton) {
                                        ((MainActivity) getActivity()).doNegativeClickN();
                                    }
                                }
                        )
                        .setNeutralButton(R.string.alert_dialog_neutral,
                                new DialogInterface.OnClickListener() {
                                    public void onClick(DialogInterface dialog, int whichButton) {
                                        ((MainActivity) getActivity()).doNeutralClickN();
                                    }
                                }
                        )
                        .create();
                break;

        }

        return builde;
    }
}


//Ста рый вариант диалога
/*public class CustomDialogFragment extends DialogFragment {
    @NonNull
//Передача в диалоговое окно стрки выделенной в списке
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        //С помощью метода getArguments() получаем переданный в MainActivity объект Bundle.
        // И так как была передана строка, то для ее извлечения применяется метод getString().
        String phone = getArguments().getString("phone");

        AlertDialog.Builder builder=new AlertDialog.Builder(getActivity());
        return builder
                .setTitle("Диалоговое окно")
                .setIcon(android.R.drawable.ic_dialog_alert)
                .setMessage("Вы хотите удалить " + phone + "?")
                .setPositiveButton("OK", null)
                .setNegativeButton("Отмена", null)
                .create();
    }

}*/