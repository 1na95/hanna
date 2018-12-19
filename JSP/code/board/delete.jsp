<%@ page import="java.sql.*" %>
<%@ page import="www.DBConnection" %>
<% String idx = request.getParameter("idx"); %>

<%
Connection conn = DBConnection.getConnection();
ResultSet rs = null;
PreparedStatement pstmt = null;
int update = 0;
boolean result = false;
String sql = "delete from tblBoard where num=?";

pstmt = conn.prepareStatement(sql);
pstmt.setString(1, idx);
update = pstmt.executeUpdate();

if(update!=0) result = true;
if(result) {%>
<script>
	alert("success");
	location.href="boardIndex.jsp";
</script>
<%} else { %>
<script>
	alert("failed");
	history.back();
</script>
<% } 
DBConnection.freeConnection(conn,pstmt);
%>
