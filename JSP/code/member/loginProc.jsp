<%@ page import="java.sql.*" %>
<%@ page import="javax.sql.*" %>
<%@ page import="www.DBConnection" %>
<%
        Connection conn = null;
        PreparedStatement pstmt = null;
        ResultSet rs = null;

        boolean result = false;
		
        String id = request.getParameter("id");
        String pass = request.getParameter("pass");
        String sql = "select count(*) as cnt from tblMember where id=? and pass=?";

	try {
                conn = DBConnection.getConnection();
                pstmt = conn.prepareStatement(sql);
                pstmt.setString(1, id);
                pstmt.setString(2, pass);

		rs = pstmt.executeQuery();
		rs.next();

		if(rs.getString("cnt").equals("1")) {	
		result = true;
		session.setAttribute("id", id);
		session.setAttribute("pass", pass);
		}

	} catch(Exception e) {
	e.getMessage();
	} finally {
		if(rs!=null) rs.close();
                if(pstmt!=null) pstmt.close();
                if(conn!=null) conn.close();
	}

	if(result) {
		%>
		<script type="text/javascript">
		alert("Login is complete");
		location.href="../index.jsp";
		</script>
	<% } else { %>
		<script type="text/javascript">
		alert("Login is failed");
		history.back(); 
		</script>
	<% } %>
