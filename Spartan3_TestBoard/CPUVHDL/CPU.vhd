library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity CPU is
port(
	clk: in std_logic;
	led: out std_logic_vector(7 downto 0)
);
end CPU;

architecture Behavioral of CPU is
	
	signal scaled_clk : std_logic;
	
	-- Program Counter Signals
	signal pc_address : std_logic_vector(31 downto 0) := (others => '0');

	-- Instruction Memory Signals
	signal instrmem_instruction : std_logic_vector(31 downto 0) := (others => '0');
	signal instrmem_opcode : std_logic_vector(5 downto 0) := (others => '0');
	signal instrmem_rs_reg : std_logic_vector(4 downto 0) := (others => '0');
	signal instrmem_rt_reg : std_logic_vector(4 downto 0) := (others => '0');
	signal instrmem_rd_reg : std_logic_vector(4 downto 0) := (others => '0');
	signal instrmem_shmt : std_logic_vector(4 downto 0) := (others => '0');
	signal instrmem_func_code : std_logic_vector(5 downto 0) := (others => '0');
	signal instrmem_immediate : std_logic_vector(15 downto 0) := (others => '0');
	signal instrmem_immediate_sign_extended : std_logic_vector(31 downto 0) := (others => '0');

	-- Instruction Fetch / Instruction Decode Pipeline Signals
	signal if_id_instruction : std_logic_vector(31 downto 0);
	signal if_id_opcode : std_logic_vector(5 downto 0);
	signal if_id_func_code : std_logic_vector(5 downto 0);
	signal if_id_rs_reg : std_logic_vector(4 downto 0) := (others => '0');
	signal if_id_rt_reg : std_logic_vector(4 downto 0) := (others => '0');
	signal if_id_rd_reg : std_logic_vector(4 downto 0) := (others => '0');
	signal if_id_shmt : std_logic_vector(4 downto 0) := (others => '0');
	signal if_id_immediate : std_logic_vector(15 downto 0) := (others => '0');
	signal if_id_immediate_sign_extended : std_logic_vector(31 downto 0) := (others => '0');

	-- Control Logic Signals
	signal ctrl_logic_reg_write_en : std_logic := '0';
	signal ctrl_logic_is_immediate_cmd : std_logic := '0';
	signal ctrl_logic_is_branch_cmd : std_logic := '0';
	signal ctrl_logic_data_mem_write_en : std_logic := '0';
	signal ctrl_logic_data_mem_read_en : std_logic := '0';
	signal ctrl_logic_mem_to_reg : std_logic := '0';
	signal ctrl_logic_alu_ctrl_line: std_logic_vector(3 downto 0) := (others => '0');

	-- Register File Signals
	signal regfile_write_reg : std_logic_vector(4 downto 0) := (others => '0');
	signal regfile_rs_val : std_logic_vector(31 downto 0) := (others => '0');
	signal regfile_rt_val : std_logic_vector(31 downto 0) := (others => '0');
	signal regfile_is_successfull_branch : std_logic := '0';
	
	-- Instruction Decode / Execute Pipeline Signals
	signal id_ex_reg_write_en : std_logic := '0';
	signal id_ex_is_immediate_cmd : std_logic := '0';
	signal id_ex_is_branch_cmd : std_logic := '0';
	signal id_ex_data_mem_write_en : std_logic := '0';
	signal id_ex_data_mem_read_en : std_logic := '0';
	signal id_ex_mem_to_reg : std_logic := '0';
	signal id_ex_alu_ctrl_line: std_logic_vector(3 downto 0) := (others => '0');
	
	signal id_ex_if_id_rs_reg : std_logic_vector(4 downto 0) := (others => '0');
	signal id_ex_if_id_rt_reg : std_logic_vector(4 downto 0) := (others => '0');
	signal id_ex_if_id_shmt : std_logic_vector(4 downto 0) := (others => '0');
	signal id_ex_if_id_immediate: std_logic_vector(15 downto 0) := (others => '0');
	signal id_ex_if_id_immediate_sign_extended: std_logic_vector(31 downto 0) := (others => '0');
	signal id_ex_write_reg : std_logic_vector(4 downto 0) := (others => '0');
	signal id_ex_rs_val : std_logic_vector(31 downto 0) := (others => '0');
	signal id_ex_rt_val : std_logic_vector(31 downto 0) := (others => '0');

	-- ALU Signals
	signal forward_a_reg : std_logic_vector(1 downto 0);
	signal forward_b_reg : std_logic_vector(1 downto 0);
	signal alu_alu_out : std_logic_vector(31 downto 0);
	signal alu_less_than : std_logic := '0';
	signal alu_mux_forward_rs_val : std_logic_vector(31 downto 0) := (others => '0');
	signal alu_mux_forward_rt_val : std_logic_vector(31 downto 0) := (others => '0');
	signal alu_mux_rt_val : std_logic_vector(31 downto 0) := (others => '0');
	
	-- Execute / Memory Pipeline Signals
	signal ex_mem_alu_out : std_logic_vector(31 downto 0) := (others => '0');
	signal ex_mem_less_than : std_logic := '0';
	signal ex_mem_mux_rt_val : std_logic_vector(31 downto 0) := (others => '0');
	
	signal ex_mem_id_ex_rt_val : std_logic_vector(31 downto 0) := (others => '0');
	signal ex_mem_id_ex_write_reg : std_logic_vector(4 downto 0) := (others => '0');
	signal ex_mem_id_ex_reg_write_en : std_logic := '0';
	signal ex_mem_id_ex_data_mem_write_en : std_logic := '0';
	signal ex_mem_id_ex_data_mem_read_en : std_logic := '0';
	signal ex_mem_id_ex_mem_to_reg : std_logic := '0';
	
	signal ex_mem_id_ex_if_id_immediate : std_logic_vector(15 downto 0) := (others => '0');
	signal ex_mem_id_ex_if_id_immediate_sign_extended: std_logic_vector(31 downto 0) := (others => '0');

	-- Data Memory Signals
	signal datamem_read_val : std_logic_vector(31 downto 0) := (others => '0');

	-- Memory / Write Back Pipeline Signals
	signal mem_wb_regfile_write_val : std_logic_vector(31 downto 0) := (others => '0');
	signal mem_wb_ex_mem_id_ex_mem_to_reg : std_logic := '0';
	signal mem_wb_ex_mem_id_ex_reg_write_en : std_logic := '0';
	signal mem_wb_ex_mem_id_ex_write_reg : std_logic_vector(4 downto 0) := (others => '0');
	signal mem_wb_ex_mem_alu_out : std_logic_vector(31 downto 0) := (others => '0');
	signal mem_wb_mem_read_val : std_logic_vector(31 downto 0) := (others => '0');

begin

	clock_divider: entity work.ClockDivider
	port map(
		clk => clk,
		scaled_clk => scaled_clk
	);

	pc: entity work.ProgramCounter
	port map(
		address => pc_address,
		clk => scaled_clk,
		is_successfull_branch => regfile_is_successfull_branch,
		address_change_input => if_id_immediate_sign_extended
	);
	
	instr_mem: entity work.InstructionMem
	port map(
		pc_address => pc_address,
		instruction => instrmem_instruction,
		opcode => instrmem_opcode,
		rs_reg => instrmem_rs_reg,
		rt_reg => instrmem_rt_reg,
		rd_reg => instrmem_rd_reg,
		shmt => instrmem_shmt,
		func_code => instrmem_func_code,
		immediate => instrmem_immediate
	);
	
	instrmem_immediate_sign_extended <= std_logic_vector(resize(signed(instrmem_immediate), instrmem_immediate_sign_extended'length));
	
	if_id: process(clk)
	begin
		if rising_edge(clk) then
			if_id_instruction <= instrmem_instruction;
			if_id_opcode <= instrmem_opcode;
			if_id_rs_reg <= instrmem_rs_reg;
			if_id_rt_reg <= instrmem_rt_reg;
			if_id_rd_reg <= instrmem_rd_reg;
			if_id_shmt <= instrmem_shmt;
			if_id_func_code <= instrmem_func_code;
			if_id_immediate <= instrmem_immediate;
			if_id_immediate_sign_extended <= instrmem_immediate_sign_extended;
		end if;
	end process;
	
	ctrl_logic: entity work.ControlLogic
	port map(
		opcode => if_id_opcode,
		func_code => if_id_func_code,
		is_branch_cmd => ctrl_logic_is_branch_cmd,
		reg_write_en => ctrl_logic_reg_write_en,
		data_mem_write_en => ctrl_logic_data_mem_write_en,
		data_mem_read_en => ctrl_logic_data_mem_read_en,
		is_immediate_cmd => ctrl_logic_is_immediate_cmd,
		mem_to_reg => ctrl_logic_mem_to_reg,
		alu_ctrl_line => ctrl_logic_alu_ctrl_line
	);
	
	regfile_write_reg <= if_id_rd_reg when ctrl_logic_is_immediate_cmd = '0' else if_id_rt_reg;
	mem_wb_regfile_write_val <= mem_wb_mem_read_val when mem_wb_ex_mem_id_ex_mem_to_reg = '0' else mem_wb_ex_mem_alu_out;

	reg_file: entity work.RegFile
	port map(
		clk => scaled_clk,
		rs_reg => if_id_rs_reg,
		rs_val => regfile_rs_val,
		rt_reg => if_id_rt_reg,
		rt_val => regfile_rt_val,
		write_reg => mem_wb_ex_mem_id_ex_write_reg,
		write_val => mem_wb_regfile_write_val,
		wen => mem_wb_ex_mem_id_ex_reg_write_en
	);
	
	regfile_is_successfull_branch <= '1' when (ctrl_logic_is_branch_cmd = '1' and regfile_rs_val = regfile_rt_val) else '0';
	
	id_ex: process(scaled_clk)
	begin
		if rising_edge(scaled_clk) then			
			if regfile_is_successfull_branch = '1' then
				id_ex_write_reg <= "00000";
				id_ex_if_id_rs_reg <= "00000";
				id_ex_if_id_rt_reg <= "00000";
				id_ex_if_id_shmt <= "00000";
				id_ex_if_id_immediate <= x"0000";
				id_ex_if_id_immediate_sign_extended <= x"00000000";

				id_ex_rs_val <= x"00000000";
				id_ex_rt_val <= x"00000000";
				
				id_ex_reg_write_en <= '0';
				id_ex_is_immediate_cmd <= '0';
				id_ex_is_branch_cmd <= '0';
				id_ex_data_mem_write_en <= '0';
				id_ex_data_mem_read_en <= '0';
				id_ex_mem_to_reg <= '1';
				id_ex_alu_ctrl_line <= x"0";
			else
				id_ex_reg_write_en <= ctrl_logic_reg_write_en;
				id_ex_is_immediate_cmd <= ctrl_logic_is_immediate_cmd;
				id_ex_is_branch_cmd <= ctrl_logic_is_branch_cmd;
				id_ex_data_mem_write_en <= ctrl_logic_data_mem_write_en;
				id_ex_data_mem_read_en <= ctrl_logic_data_mem_read_en;
				id_ex_mem_to_reg <= ctrl_logic_mem_to_reg;
				id_ex_alu_ctrl_line <= ctrl_logic_alu_ctrl_line;

				id_ex_write_reg <= regfile_write_reg;
				id_ex_rs_val <= regfile_rs_val;
				id_ex_rt_val <= regfile_rt_val;

				id_ex_if_id_rs_reg <= if_id_rs_reg;
				id_ex_if_id_rt_reg <= if_id_rt_reg;

				id_ex_if_id_shmt <= if_id_shmt;
				id_ex_if_id_immediate <= if_id_immediate;
				id_ex_if_id_immediate_sign_extended <= if_id_immediate_sign_extended;
			end if;
		end if;
	end process;
	
	forward_a_reg <=	"01" when (ex_mem_id_ex_reg_write_en = '1' and ex_mem_id_ex_write_reg = id_ex_if_id_rs_reg) else
							"10" when (mem_wb_ex_mem_id_ex_reg_write_en = '1' and mem_wb_ex_mem_id_ex_write_reg = id_ex_if_id_rs_reg) else
							"00";

	forward_b_reg <=	"01" when (ex_mem_id_ex_reg_write_en = '1' and ex_mem_id_ex_write_reg = id_ex_if_id_rt_reg) else
							"10" when (mem_wb_ex_mem_id_ex_reg_write_en = '1' and mem_wb_ex_mem_id_ex_write_reg = id_ex_if_id_rt_reg) else
							"00";
							
	alu_mux_forward_rs_val <= ex_mem_alu_out when forward_a_reg = "01" else mem_wb_ex_mem_alu_out when forward_a_reg = "10" else id_ex_rs_val;
	alu_mux_forward_rt_val <= ex_mem_alu_out when forward_b_reg = "01" else mem_wb_ex_mem_alu_out when forward_b_reg = "10" else id_ex_rt_val;
	
	alu_mux_rt_val <= id_ex_if_id_immediate_sign_extended when id_ex_is_immediate_cmd = '1' else alu_mux_forward_rt_val;
	
	alu: entity work.ALU
	port map(
		alu_ctrl_line => id_ex_alu_ctrl_line,
		rs_val => alu_mux_forward_rs_val,
		rt_val => alu_mux_rt_val,
		shmt => id_ex_if_id_shmt,
		less_than => alu_less_than,
		alu_out => alu_alu_out
	);


	ex_mem: process(scaled_clk)
	begin
		if rising_edge(scaled_clk) then
			ex_mem_alu_out <= alu_alu_out;
			ex_mem_less_than <= alu_less_than;

			ex_mem_id_ex_write_reg <= id_ex_write_reg;
			ex_mem_id_ex_rt_val <= id_ex_rt_val;
			ex_mem_id_ex_reg_write_en <= id_ex_reg_write_en;
			ex_mem_id_ex_data_mem_write_en <= id_ex_data_mem_write_en;
			ex_mem_id_ex_data_mem_read_en <= id_ex_data_mem_read_en;
			ex_mem_id_ex_mem_to_reg <= id_ex_mem_to_reg;

			ex_mem_id_ex_if_id_immediate <= id_ex_if_id_immediate;
			ex_mem_id_ex_if_id_immediate_sign_extended <= id_ex_if_id_immediate_sign_extended;
		end if;
	end process;

	data_mem: entity work.DataMem
	port map(
		clk => scaled_clk,
		wen => ex_mem_id_ex_data_mem_write_en,
		addr_input => ex_mem_id_ex_if_id_immediate,
		write_val => ex_mem_id_ex_rt_val,
		read_en => ex_mem_id_ex_data_mem_read_en,
		read_val => datamem_read_val,
		led => led
	);
	
	mem_wb: process(scaled_clk)
	begin
		if rising_edge(scaled_clk) then
			mem_wb_ex_mem_id_ex_reg_write_en <= ex_mem_id_ex_reg_write_en;
			mem_wb_ex_mem_id_ex_write_reg <= ex_mem_id_ex_write_reg;
			mem_wb_ex_mem_id_ex_mem_to_reg <= ex_mem_id_ex_mem_to_reg;
			mem_wb_ex_mem_alu_out <= ex_mem_alu_out;
			mem_wb_mem_read_val <= datamem_read_val;
		end if;
	end process;

end Behavioral;

