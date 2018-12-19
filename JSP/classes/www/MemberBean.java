package www;

public class MemberBean {

	private String id;
	private String pass;
	private String name;
	private String email;
	private String zipcode;
	private String addr1;
	private String addr2;
	private String phone;

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public String getPass() {
		return pass;
	}

	public void setPass(String pass) {
		this.pass = pass;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getEmail() {
		return email;
	}

	public void setEmail(String email) {
		this.email = email;
	}

	public String getPhone() {
		return phone;
	}

	public void setPhone(String phone) {
		this.phone = phone;
	}

        public String getZipcode() {
                return zipcode;
        }

        public void setZipcode(String zipcode) {
                this.zipcode = zipcode;
        }

        public String getAddr1() {
                return addr1;
        }

        public void setAddr1(String addr1) {
                this.addr1 = addr1;
        }

        public String getAddr2() {
                return addr2;
        }

        public void setAddr2(String addr2) {
                this.addr2 = addr2;
        }

}
