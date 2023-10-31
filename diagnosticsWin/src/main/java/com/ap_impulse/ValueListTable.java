package com.ap_impulse;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Types;
import java.util.Optional;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;

/*������� ��������� �� ��. ��`���� ��������� � ��*/
public class ValueListTable {
    private static final String tableName 		  = "ValueList";	      
    private static final String id_element_Column = "numberElement";
    private static final String value_Column      = "valueElement";
    private static final String name_Column       = "nameElement";
    private static final String id_svg_Column     = "numberSvgElement";
    private static final ObservableList<ValuelistModel> parametrs;  	  

    static {
    	parametrs = FXCollections.observableArrayList(); 
        updateParametrsFromDB();  						 
    }

    public static ObservableList<ValuelistModel> getParametrs() {  
    	System.out.println(FXCollections.unmodifiableObservableList(parametrs));
        return FXCollections.unmodifiableObservableList(parametrs); 
    }

    //---------------------------������� � ��- ���������� ��� ������ �� (�������)--------------------------------------------------
    private static void updateParametrsFromDB() { 		// ������� ������� � ��
        String query = "SELECT * FROM " + tableName;    // ����� - ������ ��� � �������
    
        try (Connection connection = Database.connect()) {
            PreparedStatement statement = connection.prepareStatement(query);
            ResultSet rs = statement.executeQuery(); 
            parametrs.clear();                      
            while (rs.next()) {						
            	parametrs.add(new ValuelistModel(    
            			rs.getInt(id_element_Column),		  
                        rs.getFloat(value_Column),
                        rs.getString(name_Column),
                        rs.getString(id_svg_Column)));
            }
        } catch (SQLException e) {
          /*  Logger.getAnonymousLogger().log(
                    Level.SEVERE,
                    LocalDateTime.now() + ": Could not load Parametrs from database ");*/
            parametrs.clear();
        }
    }

    /*----------------------------������� ������� � ��-����������� ��� �������� ������ Edit (�����������), ����� ���������-------------------------------------------------------*/
    public static void update(ValuelistModel newValuelist) {
        int rows = CRUDHelper.update( 																
                tableName,																			
                new String[]{value_Column, name_Column, id_svg_Column},                               
                new Object[]{newValuelist.getValue(), newValuelist.getName(), newValuelist.getIdSVG()},
                new int[]{Types.FLOAT, Types.VARCHAR, Types.VARCHAR},								
                id_element_Column,                                                                   
                Types.INTEGER,                                                                      
                newValuelist.getId_element()                                                            
        );
        //------------------------------------------------------------------------------------------------

        if (rows == 0)
            throw new IllegalStateException("Parametr to be updated with id " + newValuelist.getId_element() + " didn't exist in database");
       		Optional<ValuelistModel> optionalParametr = getParametr(newValuelist.getId_element());
            optionalParametr.ifPresentOrElse(oldValuelist -> {     
            parametrs.remove(oldValuelist);
            parametrs.add(newValuelist);
        }, () -> {
            throw new IllegalStateException("Parametr to be updated with id " + newValuelist.getId_element() + " didn't exist in database");
        });
    }

    //--------------------������� �������� ��������, ���������� ��� �������� ��������� ------------------------------------------
    public static void insertParametr(Float value, String name, String id_svg) {
        //update database
        int id_elements= (int) CRUDHelper.create(
                tableName,
                new String[]{"valueElement", "nameElement", "numberSvgElement"},
                new Object[]{value, name, id_svg},
                new int[]{Types.FLOAT, Types.VARCHAR, Types.VARCHAR});
        //INSERT INTO ValueList (valueElement, nameElement, numberSvgElement)  VALUES (1.0, '1', '1');
        //update cache
        parametrs.add(new ValuelistModel(
        		id_elements,
        		value,
                name,
                id_svg                 
        ));
        System.out.println(parametrs + " parametrs"); 
    }
    //----------------------------------------------------------------------------------------------------------------------------

    public static void delete(int id) {
        CRUDHelper.delete(tableName, id);
        //update cache
        Optional<ValuelistModel> parametr = getParametr(id);
        parametr.ifPresent(parametrs::remove);
    }

    public static Optional<ValuelistModel> getParametr(int id) {
        for (ValuelistModel parametr : parametrs) {
            if (parametr.getId_element() == id) return Optional.of(parametr);
        }
        return Optional.empty();
    }
}