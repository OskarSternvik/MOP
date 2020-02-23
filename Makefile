.PHONY: clean All

All:
	@echo "----------Building project:[ flipflop_irq_ack - Debug ]----------"
	@cd "flipflop_irq_ack" && "$(MAKE)" -f  "flipflop_irq_ack.mk" && "$(MAKE)" -f  "flipflop_irq_ack.mk" PostBuild
clean:
	@echo "----------Cleaning project:[ flipflop_irq_ack - Debug ]----------"
	@cd "flipflop_irq_ack" && "$(MAKE)" -f  "flipflop_irq_ack.mk" clean
