package www;

// getConnection()
// freeConnection()

import java.sql.*;
import javax.sql.*;

public class DBConnection {
	public static Connection getConnection() throws SQLException {

		Connection conn = null;
		
		try {
			Class.forName("oracle.jdbc.driver.OracleDriver");
			String JDBCINFO = "jdbc:oracle:thin:@192.168.111.107:1521:orcl";
			conn = DriverManager.getConnection(JDBCINFO, "testuser", "1234");
			return conn;

		} catch(ClassNotFoundException ce) { 
			System.out.println(ce.getMessage()); 
			return null;
		}
	}

	public static void freeConnection(Connection c, PreparedStatement p, ResultSet r) {
		try {
			if(r!=null) r.close();
			if(p!=null) p.close();
			if(c!=null) c.close();
		} catch (SQLException ex) { ex.printStackTrace(); }
	}

	public static void freeConnection(Connection c, PreparedStatement p) {
                try {
                        if(p!=null) p.close();
                        if(c!=null) c.close();
                } catch (SQLException ex) { ex.printStackTrace(); }
        }

	public static void freeConnection(Connection c, Statement s, ResultSet r) {
                try {
                        if(r!=null) r.close();
                        if(s!=null) s.close();
                        if(c!=null) c.close();
                } catch (SQLException ex) { ex.printStackTrace(); }
        }

}
