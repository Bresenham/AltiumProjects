library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity ControlLogic is
port(
	opcode: in std_logic_vector(5 downto 0);
	func_code: in std_logic_vector(5 downto 0);

	is_branch_cmd: out std_logic;
	reg_write_en: out std_logic;
	data_mem_read_en: out std_logic;
	data_mem_write_en: out std_logic;
	is_immediate_cmd: out std_logic;
	mem_to_reg: out std_logic;
	alu_ctrl_line: out std_logic_vector(3 downto 0)
);
end ControlLogic;

architecture Behavioral of ControlLogic is

begin

	process(opcode, func_code)
	begin
		is_branch_cmd <= '0';
		reg_write_en <= '0';
		is_immediate_cmd <= '0';
		data_mem_write_en <= '0';
		data_mem_read_en <= '0';
		mem_to_reg <= '0';
		alu_ctrl_line <= x"0";
		case(opcode) is
			-- R-Type Command
			when "000000" =>
				reg_write_en <= '1';
				mem_to_reg <= '1';
				case(func_code) is
					-- SLL
					when "000000" => alu_ctrl_line <= x"F";
					-- AND
					when "100100" => alu_ctrl_line <= x"0";
					-- OR
					when "100101" => alu_ctrl_line <= x"1";
					-- ADD
					when "100000" => alu_ctrl_line <= x"2";
					-- XOR
					when "100110" => alu_ctrl_line <= x"5";
					-- SUB
					when "100010" => alu_ctrl_line <= x"6";
					-- SLT
					when "101010" => alu_ctrl_line <= x"7";
					-- NOR
					when "100111" => alu_ctrl_line <= x"C";

					when others => alu_ctrl_line <= x"0";
				end case;
			-- LUI
			when "001111" =>
				alu_ctrl_line <= x"4";
				is_immediate_cmd <= '1';
			-- ADDI
			when "001000" =>
				mem_to_reg <= '1';
				alu_ctrl_line <= x"2";
				reg_write_en <= '1';
				is_immediate_cmd <= '1';
			-- BEQ
			when "000100" =>
				is_branch_cmd <= '1';
				alu_ctrl_line <= x"6";
				is_immediate_cmd <= '0';
			-- LW
			when "100011" =>
				data_mem_read_en <= '1';
				alu_ctrl_line <= x"2";
				reg_write_en <= '1';
				is_immediate_cmd <= '1';
				mem_to_reg <= '0';
			-- SW
			when "101011" =>
				alu_ctrl_line <= x"2";
				is_immediate_cmd <= '1';
				data_mem_write_en <= '1';

			when others =>
				is_branch_cmd <= '0';
				data_mem_read_en <= '0';
				reg_write_en <= '0';
				is_immediate_cmd <= '0';
				data_mem_write_en <= '0';
				mem_to_reg <= '0';
		end case;
	end process;

end Behavioral;

