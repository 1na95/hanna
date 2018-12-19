package www;

import java.sql.*;

public class BoardMgr {
        public int listBoard() {

        Connection conn = null;
        Statement stmt = null;
        ResultSet rs = null;

	int total = 0;

        try {
        conn = DBConnection.getConnection();
        String sql = "select COUNT(*) from tblBoard";
        rs = stmt.executeQuery(sql);
        if(rs.next()) {
		total = rs.getInt(1);
                }
        } catch (Exception ex) {
        ex.printStackTrace();
        } finally {
        DBConnection.freeConnection(conn, stmt, rs);
        }
        return total;
        }
}

