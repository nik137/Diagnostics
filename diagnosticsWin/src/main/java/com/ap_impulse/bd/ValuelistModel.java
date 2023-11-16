package com.ap_impulse.bd;

import javafx.beans.property.FloatPropertyBase;
import javafx.beans.property.ReadOnlyFloatProperty;
import javafx.beans.property.ReadOnlyStringProperty;
import javafx.beans.property.SimpleFloatProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringPropertyBase;
import javafx.beans.value.WritableFloatValue;

//���� ������ ��� �����䳿 � ��������
public class ValuelistModel implements Comparable<ValuelistModel>{ 
	private ReadOnlyStringProperty  name ;  
    public  ReadOnlyStringProperty  id_svg ;
    private ReadOnlyFloatProperty  value ;
	private int   id_element;
	
	public ValuelistModel(int id_element, Float value, String name, String id_svg) {
		super();
		this.id_element = id_element;
		this.value = new SimpleFloatProperty(value);
		this.name = new SimpleStringProperty(name);
		this.id_svg = new SimpleStringProperty(id_svg);
	}

	@Override
	public String toString() {
		return "Valuelist [id_element=" + id_element + ", value=" + value + ", name=" + name +  ", id_svg=" + id_svg +"]";
	}
	
	public Float getValue() {
		return value.get();
	}
	
	public void setValue(Float value) {
		((FloatPropertyBase) this.value).set(value);
	}
	
	public ReadOnlyFloatProperty idProperty() {
	    return value;
	}
	
	public int getId_element() {
		return id_element;
	}
	
	public void setId_element(int id_element) {
		this.id_element = id_element;
	}

	public String getName() {
		return name.get();
	}

	public void setName(float name) {
		((WritableFloatValue) this.name).set(name); 
	}
	public ReadOnlyStringProperty nameProperty() {
        return name;
    }
	
	public void setIdSVG(String id_svg_) {
		((StringPropertyBase) id_svg).set(id_svg_);
	}
	
	public String getIdSVG() {
		return id_svg.get();
	}
	
	public ReadOnlyStringProperty id_svgProperty() {
        return id_svg;
    }
	
	@Override
	public int compareTo(ValuelistModel o) {
		// TODO Auto-generated method stub
		return 0;
	}	
}
