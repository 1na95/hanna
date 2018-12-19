package www;

import java.sql.*;

public class MemberMgr {
	public MemberBean getMember(String id) {

	Connection conn = null;
    	PreparedStatement pstmt = null;
        ResultSet rs = null;
	MemberBean bean = null;

	try {
	conn = DBConnection.getConnection();
	String sql = "select * from tblMember where id=?";
	pstmt = conn.prepareStatement(sql);
	pstmt.setString(1, id);
	rs = pstmt.executeQuery();
		if(rs.next()) {
		bean = new MemberBean();
		bean.setId(rs.getString("id"));
		bean.setPass(rs.getString("pass"));
		bean.setName(rs.getString("name"));
		bean.setEmail(rs.getString("email"));
		bean.setPhone(rs.getString("phone"));
		bean.setZipcode(rs.getString("zipcode"));
		bean.setAddr1(rs.getString("addr1"));
		bean.setAddr2(rs.getString("addr2"));
		}
	} catch (Exception ex) {
	ex.printStackTrace();
	} finally {
	DBConnection.freeConnection(conn, pstmt, rs);
	}

	return bean;
	}

	public boolean updateMember(MemberBean bean) {
	Connection conn = null;
        PreparedStatement pstmt = null;
	boolean flag = false; 

        try {
        conn = DBConnection.getConnection();

	String sql = "update tblMember set pass=?,name=?,email=?,phone=?,zipcode=?,addr1=?,addr2=? where id=?";
	pstmt = conn.prepareStatement(sql);

	pstmt.setString(1, bean.getPass());
	pstmt.setString(2, bean.getName());
	pstmt.setString(3, bean.getEmail());
	pstmt.setString(4, bean.getPhone());
	pstmt.setString(5, bean.getZipcode());
	pstmt.setString(6, bean.getAddr1());
	pstmt.setString(7, bean.getAddr2());
	pstmt.setString(8, bean.getId());

	int count = pstmt.executeUpdate();

	if(count > 0) flag = true; }
	
	catch (Exception e) { e.printStackTrace(); }

	finally { DBConnection.freeConnection(conn, pstmt); }

	return flag;
	}
}
