<html>
<head></head>

<body>

<table><tr><td><jsp:include page="../top.jsp" /></td></tr></table>

<table height="1000" border="1"><tr><td width="100" valign=top><jsp:include page="../left.jsp" /></td>
	<td width="900" valign=top><table border="1">

	<div align="center"><br><br>
	<table align="center" border="1" cellpadding="5" height="200" width="400">
	<br><br>
	<form action="loginProc.jsp" method="post">
	<tr><td align="center" valign="middle">
	<label>ID : </label>
	<input type="text" name="id"> <br><br>

	<label>PW : </label>
	<input type="password" name="pass"> <br><br>

	<input type="submit" value="LOGIN" />
	</tr></td>
	</form>
	</table>
</table>
</body>
</html>
