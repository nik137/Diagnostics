package com.example.myapplication;

/**
 * Created by Шпуряка on 17.12.2019.
 */
//это класс гета и сета, У меня свой ValuesApplication
//We will display JSON Data in ListView, so create a new class named “PlayersModel” and add following

public class PlayersModel {

    private String name, country, city;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getCountry() {
        return country;
    }

    public void setCountry(String country) {
        this.country = country;
    }

    public String getCity() {
        return city;
    }

    public void setCity(String city) {
        this.city = city;
    }
}