library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity ProgramCounter is
port(
	clk: in std_logic;
	is_successfull_branch: in std_logic;
	address_change_input: in std_logic_vector(31 downto 0);
	address: out std_logic_vector(31 downto 0)
);
end ProgramCounter;

architecture Behavioral of ProgramCounter is

	signal tmp_address: std_logic_vector(31 downto 0) := (others => '0');

begin

	process (clk)
	begin
	
		if rising_edge(clk) then
			if is_successfull_branch = '1' then
				tmp_address <= std_logic_vector(
										signed(tmp_address) +
										signed(address_change_input)
									);
			else
				tmp_address <= std_logic_vector(unsigned(tmp_address) + 1);
			end if;
		end if;
	
	end process;

	address <= tmp_address;

end Behavioral;

