library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity ClockDivider is
port(
	clk: in std_logic;
	scaled_clk: out std_logic
);
end ClockDivider;

architecture Behavioral of ClockDivider is

	signal counter : std_logic_vector(31 downto 0);
	signal tmp_scaled_clk : std_logic := '0';

begin

	process(clk)
	begin
		if rising_edge(clk) then
			counter <= std_logic_vector(unsigned(counter) + 1);
			-- Assuming 12MHz clock
			if counter = x"003D0900" then
				tmp_scaled_clk <= not tmp_scaled_clk;
				counter <= x"00000000";
			end if;
		end if;
	end process;
	
	scaled_clk <= tmp_scaled_clk;

end Behavioral;

