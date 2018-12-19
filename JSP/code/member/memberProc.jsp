<%@ page import="java.sql.*"%>
<%@ page import="javax.sql.*"%>
<%@ page import="javax.naming.*" %>
<%@ page import="www.DBConnection" %>

<%
	Connection conn = DBConnection.getConnection();
	PreparedStatement pstmt = null;
	ResultSet rs = null;
		
	boolean result = false;

	String id = request.getParameter("id");
        String pass = request.getParameter("pass");
        String name = request.getParameter("name");
        String email = request.getParameter("email");
	String zipcode = request.getParameter("zipcode");
	String addr1 = request.getParameter("addr1");
	String addr2 = request.getParameter("addr2");
        String phone = request.getParameter("phone");
	
	String sql = "insert into tblMember values (?, ?, ?, ?, ?,sysdate,?,?,?)";
	int updateCount = 0;

	try {
		pstmt = conn.prepareStatement(sql);

		pstmt.setString(1, id);
		pstmt.setString(2, pass);
		pstmt.setString(3, name);
		pstmt.setString(4, email);
		pstmt.setString(5, phone);
		pstmt.setString(6, zipcode);
		pstmt.setString(7, addr1);
		pstmt.setString(8, addr2);

		updateCount = pstmt.executeUpdate();
	
		if (updateCount != 0) {
			result = true;
		}

	} catch (Exception e) {
		e.getMessage();
	} finally {
		if (rs != null) try { rs.close();} catch(SQLException ex) {}
		if (pstmt != null) try { pstmt.close();} catch(SQLException ex) {}
		if (conn != null) try { conn.close(); } catch(SQLException ex) {}
	}
if(result) {
%>
<script type="text/javascript">
	alert("Join is complete");
//	var id ="<%=phone%>";
//	alert("id:"$phone);
	location.href="../index.jsp";

</script>
<%} else {%>
<script type="text/javascript">
	alert("Join is Failed");
	history.back();
</script>
<% } %>

