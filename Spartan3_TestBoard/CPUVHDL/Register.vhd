library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity RegFile is
port(
	clk: in std_logic;

	rs_reg: in std_logic_vector(4 downto 0);
	rs_val: out std_logic_vector(31 downto 0);
	
	rt_reg: in std_logic_vector(4 downto 0);
	rt_val: out std_logic_vector(31 downto 0);
	
	write_reg: in std_logic_vector(4 downto 0);
	write_val: in std_logic_vector(31 downto 0);
	wen: in std_logic
);
end RegFile;

architecture Behavioral of RegFile is

	type reg_type is array (0 to 31) of std_logic_vector(31 downto 0);
	signal reg : reg_type := (others => (others => '0'));

begin

	rs_val <= write_val when rs_reg = write_reg else reg(to_integer(unsigned(rs_reg)));
	rt_val <= write_val when rt_reg = write_reg else reg(to_integer(unsigned(rt_reg)));
	
	process(clk)
	begin
		if rising_edge(clk) then
			if wen = '1' then
				reg(to_integer(unsigned(write_reg))) <= write_val;
			end if;
		end if;
	end process;


end Behavioral;

