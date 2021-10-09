library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity DataMem is
port(
	clk: in std_logic;
	wen: in std_logic;
	write_val: in std_logic_vector(31 downto 0);
	
	read_en: in std_logic;
	read_val: out std_logic_vector(31 downto 0);
	
	addr_input: in std_logic_vector(15 downto 0);
	
	led: out std_logic_vector(7 downto 0)
);
end DataMem;

architecture Behavioral of DataMem is

	type mem is array (0 to 15) of std_logic_vector(31 downto 0);
	signal data_mem : mem := (others => (others => '0'));
	signal tmp_read_val : std_logic_vector(31 downto 0) := (others => '0');

begin
	
	read_val <= tmp_read_val;
	
	process (clk)
	begin
		if rising_edge(clk) then
			if wen = '1' then
				if addr_input = x"2000" then
					led <= write_val(7 downto 0);
				else
					data_mem(to_integer(unsigned(addr_input))) <= write_val;
				end if;
			end if;
			
			if read_en = '1' then
				tmp_read_val <= data_mem(to_integer(unsigned(addr_input)));
			end if;
		end if;
	end process;

end Behavioral;

