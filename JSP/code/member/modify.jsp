<%@ page import="java.sql.*" %>
<%@ page import="javax.sql.*" %>
<%@ page import="www.DBConnection" %>
<%@ page import="www.MemberBean" %>

<jsp:useBean id="mMgr" class="www.MemberMgr" />

<%
	String id = (String)session.getAttribute("id");

	MemberBean mBean = mMgr.getMember(id);

	String pass = mBean.getPass();
	String name = mBean.getName();
	String email = mBean.getEmail();
	String zipcode = mBean.getZipcode();
	String addr1 = mBean.getAddr1();
	String addr2 = mBean.getAddr2();
	String phone = mBean.getPhone();
%>
<script>
function checkPost() {
window.open("modifyPost.jsp", "postForm", "width=500, height=500, scrollbar=yes");
}
</script>

<html>
<head>
<title>Main Page</title>
</head>

<body>

<table border="1">
<table><tr><td><jsp:include page="../top.jsp" /></td></tr></table>

<table height="1000" border="1"><tr><td width="100" valign=top><jsp:include page="../left.jsp" /></td>

	<td width="900" valign=top><table border="1">

	<div align="center"><br><br>
	<table align="center" border="1" cellpadding="5" height="200" width="300"><br><br>
	<form name="mdfFrm" action="modifyProc.jsp" method="post">
	<tr><td align="center" valign="middle">
	<b>my information</b><br></td></tr>
	
	<tr><td><label>ID : <input id=id name="id" type="text" readonly value="<%=id%>"></label></td></tr>
	<tr><td><label>Password : <input id=pass name="pass" type="text" value="<%=pass%>"></tr></td>
	<tr><td><label>Name : <input id=name name="name" type="text" value="<%=name%>"></tr></td>
	<tr><td><label>Email : <input id=email name="email" type="text" value="<%=email%>"></tr></td>
	<tr><td>Addr : <input type id=zipcode name="zipcode" type="text" readonly value="<%=zipcode%>"><input type="button" value="search" onClick="javascript:checkPost()"/><br />
	<input type="text" id="addr1" name="addr1" readonly value="<%=addr1%>"/><br />
	<input type="text" id="addr2" name="addr2" value="<%=addr2%>"/>

	<tr><td><label>phone : <input id=phone name="phone" type="text" value="<%=phone%>"></tr></td>

	<tr><td align="center"><input type="submit" value="modify"></tr></td>

</form>
</table>
</body>
</html>
