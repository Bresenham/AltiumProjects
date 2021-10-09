library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity ALU is
port(
	alu_ctrl_line: in std_logic_vector(3 downto 0);
	rs_val: in std_logic_vector(31 downto 0);
	rt_val: in std_logic_vector(31 downto 0);
	shmt: in std_logic_vector(4 downto 0);
	
	less_than: out std_logic;
	alu_out: out std_logic_vector(31 downto 0)
);
end ALU;

architecture Behavioral of ALU is

begin

	process(alu_ctrl_line, rs_val, rt_val)
	begin
		less_than <= '0';
		alu_out <= x"00000000";
		case(alu_ctrl_line) is
			-- AND
			when x"0" => alu_out <= rs_val and rt_val;
			-- OR
			when x"1" => alu_out <= rs_val or rt_val;
			-- ADD / ADDI / LW / SW
			when x"2" => alu_out <= std_logic_vector(signed(rs_val) + signed(rt_val));
			-- LUI
			when x"4" => alu_out <= rt_val(31 downto 16) & x"0000";
			-- XOR
			when x"5" => alu_out <= rs_val xor rt_val;
			-- SUB / BEQ
			when x"6" => alu_out <= std_logic_vector(signed(rs_val) - signed(rt_val));
			-- SLT
			when x"7" =>
				if signed(rs_val) < signed(rt_val) then
					less_than <= '1';
				else
					less_than <= '0';
				end if;
			-- NOR
			when x"C" => alu_out <= rs_val nor rt_val;
			-- SLL
			when x"F" => alu_out <= std_logic_vector(shift_left(unsigned(rt_val), to_integer(unsigned(shmt))));

			when others =>
				alu_out <= x"00000000";
				less_than <= '0';
		end case;
	end process;

end Behavioral;

