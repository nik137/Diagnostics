package diagnosticsWin;

import java.sql.*;
import java.time.LocalDateTime;
import java.util.logging.Level;
import java.util.logging.Logger;
/*CRUDHelper - запити до БД. Відокремлює всю структуру конкретного SQL-запиту від об’єктів доступу.
 *CRUD (FOR MySQL - create, read, update, delete)/(FOR SQL - insert, select, update, delete)
 *Підтримка FLOAT, INTEGER, VARCHAR*/
public class CRUDHelper {
    /* Читання. 
     * Read "Select fieldName from tableName where indexFieldName = convertObjectToSQLField(index, indexDataType)"
     */
    public static Object read(String tableName, String fieldName, int fieldDataType,
                              String indexFieldName, int indexDataType, Object index) {
    	//Form a request
        StringBuilder queryBuilder = new StringBuilder("Select ");
        queryBuilder.append(fieldName);
        queryBuilder.append(" from ");
        queryBuilder.append(tableName);
        queryBuilder.append(" where ");
        queryBuilder.append(indexFieldName);
        queryBuilder.append(" = ");
        queryBuilder.append(convertObjectToSQLField(index, indexDataType));
        //Визначаєм тип відповіді запиту. I define the type of response to the request
        try (Connection connection = Database.connect()) {
            PreparedStatement statement = connection.prepareStatement(queryBuilder.toString());
            try (ResultSet rs = statement.executeQuery()) {
                rs.next();
                switch (fieldDataType) {
                	case Types.FLOAT:
                		return rs.getFloat(fieldName);
                    case Types.INTEGER:
                        return rs.getInt(fieldName);
                    case Types.VARCHAR:
                        return rs.getString(fieldName);
                    default:
                        throw new IllegalArgumentException("Index type " + indexDataType + " from sql.Types is not yet supported.");
                }
            }
        } catch (SQLException exception) {
            Logger.getAnonymousLogger().log(
                    Level.SEVERE,
                    LocalDateTime.now() + ": Could not fetch from " + tableName + " by index " + index +
                            " and column " + fieldName);
            return null;
        }
    }

    /* Оновлення
     * Update "UPDATE tableName WHERE indexFieldName = convertObjectToSQLField(index, indexDataType)
     * SET columns[i] = convertObjectToSQLField(values[i], types[i])" 
     * */
    public static int update(String tableName, String[] columns, Object[] values, int[] types,
                             String indexFieldName, int indexDataType, Object index) {
        int number = Math.min(Math.min(columns.length, values.length), types.length);
        StringBuilder queryBuilder = new StringBuilder("UPDATE " + tableName + " WHERE ");
        queryBuilder.append(indexFieldName);
        queryBuilder.append(" = ");
        queryBuilder.append(convertObjectToSQLField(index, indexDataType));
        queryBuilder.append(" SET ");
        for (int i = 0; i < number; i++) {
            queryBuilder.append(columns[i]);
            queryBuilder.append(" = ");
            queryBuilder.append(convertObjectToSQLField(values[i], types[i]));
            if (i < number - 1) queryBuilder.append(", ");
        }
        try (Connection conn = Database.connect()) {
            PreparedStatement pstmt = conn.prepareStatement(queryBuilder.toString());
            System.out.println(pstmt);
            return pstmt.executeUpdate(); //number of affected rows
        } catch (SQLException ex) {
            Logger.getAnonymousLogger().log(
                    Level.SEVERE,
                    LocalDateTime.now() + ": Could not add parametr to database");
            return -1;
        }
    }
    
    /* Створення.
     * Create "INSERT INTO  + tableName +  (columns[i],)  VALUES ( values[i],); "
     * */
    public static long create(String tableName, String[] columns, Object[] values, int[] types) {
        int number = Math.min(Math.min(columns.length, values.length), types.length);
        StringBuilder queryBuilder = new StringBuilder("INSERT INTO " + tableName + " (");
        for (int i = 0; i < number; i++) {
            queryBuilder.append(columns[i]);
            if (i < number - 1) queryBuilder.append(", ");
        }
        queryBuilder.append(") ");
        queryBuilder.append(" VALUES (");
        for (int i = 0; i < number; i++) {
            switch (types[i]) {
                case Types.VARCHAR:
                    queryBuilder.append("'");
                    queryBuilder.append((String) values[i]);
                    queryBuilder.append("'");
                    break;
                case Types.INTEGER:
                    queryBuilder.append((int) values[i]);
                case Types.FLOAT:
                    queryBuilder.append((float) values[i]);
            }
            if (i < number - 1) queryBuilder.append(", ");
        }
        queryBuilder.append(");");
        System.out.println(queryBuilder);

        try (Connection conn = Database.connect()) {
            PreparedStatement pstmt = conn.prepareStatement(queryBuilder.toString());
            int affectedRows = pstmt.executeUpdate();
            // check the affected rows
            if (affectedRows > 0) {
                // get the ID back
                try (ResultSet rs = pstmt.getGeneratedKeys()) {
                    if (rs.next()) {
                        return rs.getLong(1);
                    }
                }
            }
        } catch (SQLException ex) {
            Logger.getAnonymousLogger().log(
                    Level.SEVERE,
                    LocalDateTime.now() + ": Could not add person to database");
            return -1;
        }
        return -1;
    }

    
    /* Видалення.
     * DELETE  "DELETE FROM  tableName  WHERE numberElement = ?"
     * */
    public static int delete(String tableName, int numberElement) {
        String sql = "DELETE FROM " + tableName + " WHERE numberElement = ?";

        try (Connection conn = Database.connect()) {
            PreparedStatement pstmt = conn.prepareStatement(sql);
            System.out.println(sql);
            System.out.println(pstmt);
            pstmt.setInt(1, numberElement);
            System.out.println(numberElement);
            return pstmt.executeUpdate();

        } catch (SQLException e) {
            Logger.getAnonymousLogger().log(
                    Level.SEVERE,
                    LocalDateTime.now() + ": Could not delete from " + tableName + " by numberElement " + numberElement +
                            " because " + e.getCause());
            return -1;
        }
    }

    /*Визначення передаваємого типу. Definition of the transfer type*/
    private static String convertObjectToSQLField(Object value, int type) {
        StringBuilder queryBuilder = new StringBuilder();
        switch (type) {
            case Types.VARCHAR:
                queryBuilder.append("'");
                queryBuilder.append(value);
                queryBuilder.append("'");
                break;
            case Types.INTEGER:
                queryBuilder.append(value);
                break;
            case Types.FLOAT:
                queryBuilder.append(value);
                break;
            default:
                throw new IllegalArgumentException("Index type " + type + " from sql.Types is not yet supported.");
        }
        return queryBuilder.toString();
    }
}