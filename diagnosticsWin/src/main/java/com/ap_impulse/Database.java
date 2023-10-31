package com.ap_impulse;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.time.LocalDateTime;
import java.util.logging.Level;
import java.util.logging.Logger;


/* ���� ������������, ���������� ��������, ������ �������� � ��, ��������� �������� �������.
 * - ������������ ����� getResource("/database.db")
 * - ��� ������� requiredTable = "ValueList"
 * - �������� �������� 
 * - �������� ���������� �������� JavaFX �� ����� ���� ����� SQLite
 * - �������� ������� � ��
 *  */

public class Database {
    /**
     * Location of database
     */
	//������������ ����� ����� �� ����� ���� �����.
	//��������� ������� �� �� � ����� �������.
    private static final String location = Main.class.getResource("/database.db").toExternalForm();

    /**
     * Currently only table needed
     */
    private static final String requiredTable = "ValueList";

    public static boolean isOK() {
        if (!checkDrivers()) return false; 	  //driver errors
        if (!checkConnection()) return false; //can't connect to db
        return checkTables();                 //tables didn't exist
    }

    private static boolean checkDrivers() {
        try {
            Class.forName("org.sqlite.JDBC");                    // ������������ ����� org.sqlite.JDBC �� ��������� Java ClassLoader
            DriverManager.registerDriver(new org.sqlite.JDBC()); // �������� ������� �� ��������� �ᒺ��� DriverManage
            return true;
        } catch (ClassNotFoundException | SQLException classNotFoundException) {
            Logger.getAnonymousLogger().log(Level.SEVERE, LocalDateTime.now() + ": Could not start SQLite Drivers");
            return false;
        }
    }

    private static boolean checkConnection() {
        try (Connection connection = connect()) {
            return connection != null;
        } catch (SQLException e) {
            Logger.getAnonymousLogger().log(Level.SEVERE, LocalDateTime.now() + ": Could not connect to database");
            return false;
        }
    }
    
    
    /*���������� �������� ������� � ��.*/
    private static boolean checkTables() {
        String checkTables =
                "select DISTINCT tbl_name from sqlite_master where tbl_name = '" + requiredTable + "'";
        try (Connection connection = Database.connect()) {
            PreparedStatement statement = connection.prepareStatement(checkTables);
            ResultSet rs = statement.executeQuery();
            while (rs.next()) {
                if (rs.getString("tbl_name").equals(requiredTable)) return true;
            }
        } catch (SQLException exception) {
            Logger.getAnonymousLogger().log(Level.SEVERE, LocalDateTime.now() + ": Could not find tables in database");
            return false;
        }
        return false;
    }

    protected static Connection connect() {
        String dbPrefix = "jdbc:sqlite:";
        Connection connection;
        try {
        	//�������� jdbc ��� SQLite. ���������� � �������������� jdbc:sqlite: ���� ������������ ����� ���� �����
            connection = DriverManager.getConnection(dbPrefix + location);
        } catch (SQLException exception) {
            Logger.getAnonymousLogger().log(Level.SEVERE,
                    LocalDateTime.now() + ": Could not connect to SQLite DB at " + location);
            return null;
        }
        return connection;
    }
    
}
