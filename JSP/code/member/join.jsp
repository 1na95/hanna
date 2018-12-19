<%
        String cPath=request.getContextPath();
%>
<script type="text/javascript">
function checkPost() {
window.name = "joinForm";
window.open("checkPost.jsp", "postForm", "width=500, height=500, scrollbar=yes");
}
</script>
<html>
<head>
<title>Join Page</title>
</head>
<body>
<table>
        <tr>
                <td><jsp:include page="../top.jsp" /></td>
        </tr>
</table>
<table height="1000" border="1">
        <tr>
                <td width="100" valign=top><jsp:include page="../left.jsp" /></td>

                <td width="900" valign=top>

	<div align="center">
		<br /><br />
		<form name="regFrm" method="post" action="memberProc.jsp">
			<table align="center" cellpadding="5">
			<tr><td align="center" valign="middle">
				<table border="1" cellpadding="2" align="center" width="600">
				<tr align="center"><td colspan="3"><b>JOIN</b></td></tr>

				<tr><td width="20%">ID</td>
				<td width="50%"><input type="text" name="id" size="10" value="" maxlength="10"/></td>
				<td width="30%">please fill in ID and double check.</td></tr>

				<tr><td>Password</td>
				<td><input type="password" name="pass" size="10" value="" maxlength="10"></td>
				<td>Please fill in Password.</td></tr>


				<tr><td>Name</td>
				<td><input type="text" name="name" size="10" value="" maxlength="10"></td>
				<td>Please fill in your name.</td></tr>

				<tr><td>Email</td>
				<td><input type="text" name="email" size="10" value="" maxlength="10"></td>
				<td>Please fill in your email.</td></tr>
			
				<tr><td>Address</td>
				<td><input type="text" id="zipcode" name="zipcode" size="10" maxlength="10" readonly /> &nbsp; <input type="button" value="search" onClick="javascript:checkPost()"/><br /><br/>
				<input type="text" id="addr1" name="addr1" readonly/><br /><br />
				<input type="text" id="addr2" name="addr2" />

				<tr><td>Phone</td>
				<td><input type="text" name="phone" size="10" value="" maxlength="10">	</td>
				<td>Please fill in your phone number.</td></tr>

				<tr><td colspan="3" align="center">
				<input type="submit"value="JOIN"> &nbsp; &nbsp; 
				<input type="reset" value="CLEAR"></td></tr>
			</table>
		</td></tr>
		</table>
	</form>
</td></tr></table>
</body>
</html>
