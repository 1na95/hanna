<%@ page import="java.sql.*" %>
<%@ page import="www.DBConnection" %>
<%
Connection conn = DBConnection.getConnection();
PreparedStatement pstmt;
ResultSet rs;

String ro = request.getParameter("doroname");

String sql = "select zipcode, sido, ro, jibun from addr where ro like'%"+ro+"%'";
pstmt = conn.prepareStatement(sql);
rs = pstmt.executeQuery();

%>
<html><head>
<title>zipcode</title>
<script type="text/javascript">
function checkPostClose(zipcode, address) {
	opener.regFrm.zipcode.value = zipcode;
	opener.regFrm.addr1.value = address;
	window.close();
	opener.regFrm.addr2.focus();
}

function checkSearch() {
	if(document.checkPostForm.doroname.value=="") { alert("Please input the doroname"); }
	else document.checkPostForm.submit();
}
</script>
</head>
<body>
<form name="checkPostForm" method="post" action="checkPost.jsp">
<center>doroname : <input type="text" name="doroname" id="doroname"/>
<input type="button" name="search" value="search" onClick="javascript:checkSearch()" /><br />
<table border="1" width="100%"><th width="15%">zipcode</th>
<th>address</th>

<%
if(rs==null) out.println("no address");
String address;
String zipcode;
while(rs.next()) {
	address = rs.getString("sido") +" "+ rs.getString("ro") +" "+ rs.getString("jibun");
	zipcode = rs.getString("zipcode");

%>

<tr><td width="15%"><a href="#" onClick="checkPostClose('<%=zipcode%>','<%=address%>')"><%=zipcode%></a></td>
<td align="left"><a href="#" onClick="checkPostClose('<%=zipcode%>','<%=address%>')"><%=address%></a></td></tr>

<% }  DBConnection.freeConnection(conn, pstmt, rs); %>
</table></center></form></body></html>
