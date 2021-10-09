library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity InstructionMem is
port(
	pc_address: in std_logic_vector(31 downto 0);

	instruction: out std_logic_vector(31 downto 0);
	opcode: out std_logic_vector(5 downto 0);
	rs_reg: out std_logic_vector(4 downto 0);
	rt_reg: out std_logic_vector(4 downto 0);
	rd_reg: out std_logic_vector(4 downto 0);
	shmt: out std_logic_vector(4 downto 0);
	func_code: out std_logic_vector(5 downto 0);
	immediate: out std_logic_vector(15 downto 0)
);
end InstructionMem;

architecture Behavioral of InstructionMem is

	type mem is array(0 to 31) of std_logic_vector(31 downto 0);
	signal instr_mem : mem :=(
		0 => x"20090000",
		1 => x"200a0001",
		2 => x"200b0000",
		3 => x"01495820",
		4 => x"21490000",
		5 => x"216a0000",
		6 => x"ac0b2000",
		7 => x"1000fffb",
		others => (others=>'0')
	);

begin
	
	instruction <= instr_mem(to_integer(unsigned(pc_address)));

	opcode <= instr_mem(to_integer(unsigned(pc_address)))(31 downto 26);
	rs_reg <= instr_mem(to_integer(unsigned(pc_address)))(25 downto 21);
	rt_reg <= instr_mem(to_integer(unsigned(pc_address)))(20 downto 16);
	rd_reg <= instr_mem(to_integer(unsigned(pc_address)))(15 downto 11);
	shmt <= instr_mem(to_integer(unsigned(pc_address)))(10 downto 6);
	func_code <= instr_mem(to_integer(unsigned(pc_address)))(5 downto 0);
	
	immediate <= instr_mem(to_integer(unsigned(pc_address)))(15 downto 0);

end Behavioral;

