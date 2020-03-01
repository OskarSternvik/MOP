.PHONY: clean All

All:
	@echo "----------Building project:[ Pong_game - Debug ]----------"
	@cd "Pong_game" && "$(MAKE)" -f  "Pong_game.mk" && "$(MAKE)" -f  "Pong_game.mk" PostBuild
clean:
	@echo "----------Cleaning project:[ Pong_game - Debug ]----------"
	@cd "Pong_game" && "$(MAKE)" -f  "Pong_game.mk" clean
